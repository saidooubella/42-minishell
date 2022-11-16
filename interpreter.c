/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:20:00 by soubella          #+#    #+#             */
/*   Updated: 2022/11/16 11:53:20 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "string_builder.h"
#include "exec_resolver.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "builtins.h"
#include "parser.h"
#include "utils.h"
#include "nodes.h"

// --------------------------------------------------------

char	**unwrap_args(t_string *args, size_t size)
{
	size_t	index;
	char	**mapped;
	
	mapped = malloc(sizeof(char *) * (size + 1));
	if (mapped == NULL)
		memory_error();
	index = -1;
	while (++index < size)
		mapped[index] = args[index].value;
	mapped[index] = NULL;
	return (mapped);
}

char	**unwrap_env(t_symbol *syms, size_t size)
{
	size_t	index;
	char	**mapped;
	
	mapped = malloc(sizeof(char *) * (size + 1));
	if (mapped == NULL)
		memory_error();
	index = -1;
	while (++index < size)
	{
		char *str1 = string_join(syms[index].name.value, "=");
		char *str2 = string_join(str1, syms[index].value.value);
		mapped[index] = (free(str1), str2);
	}
	mapped[index] = NULL;
	return (mapped);
}

int	await_process(int pid)
{
	int status;

	status = 0;
	// ft_printf(STDERR_FILENO, "await for... %d\n", pid);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WTERMSIG(status) + 128 == 130)
		write (1, "\n", 1);
	return (WTERMSIG(status) + 128);
}

void	close_fd(int fd)
{
	if (fd == -1)
		return ;
	close(fd);
	// ft_printf(STDERR_FILENO, "closed(%d) from %d\n", fd, getpid());
}

int	duplicate_fd(int fd)
{
	int	new;

	if (fd != -1)
		new = dup(fd);
	else
		new = -1;
	// ft_printf(STDERR_FILENO, "dup(%d -> %d) from %d\n", fd, new, getpid());
	return (new);
}

void	redirect_fd(int src, int dst) {
	if (src == -1)
		return ;
	dup2(src, dst);
	close_fd(src);
	// ft_printf(STDERR_FILENO, "redirect(%d -> %d) from %d\n", src, dst, getpid());
}

void	resolve_redirections(t_command_node *node, int *in, int *out, bool builtin)
{
	size_t	index;

	(void) builtin;
	index = -1;
	while (++index < node->redirections_size)
	{
		t_redirection redirect = node->redirections[index];
		if (redirect.type == INPUT) {
			close_fd(*in);
			*in = open(redirect.extra.value, O_RDONLY);
			if (*in == -1)
				error("minishell: %s: %s", strerror(errno), redirect.extra.value);
		} else if (redirect.type == OUTPUT) {
			close_fd(*out);
			*out = open(redirect.extra.value, O_WRONLY | O_TRUNC | O_CREAT, 0644);
			if (*out == -1)
				error("minishell: %s: %s", strerror(errno), redirect.extra.value);
		} else if (redirect.type == HEREDOC) {
			close_fd(*in);
			int read_write[2];
			pipe(read_write);
			write(read_write[1], redirect.extra.value, string_length(redirect.extra.value));
			close_fd(read_write[1]);
			*in = read_write[0];
		} else if (redirect.type == APPEND) {
			close_fd(*out);
			*out = open(redirect.extra.value, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (*out == -1)
				error("minishell: %s: %s", strerror(errno), redirect.extra.value);
		}
	}
}

t_result	result_create(bool success, int extra)
{
	t_result	result;

	result.success = success;
	result.extra = extra;
	return (result);
}

t_result	visit_command_node(
	t_environment *env,
	t_command_node *node,
	int in, int out, int to_be_closed,
	bool should_wait
)
{	
	if (node->args_size > 0)
	{
		t_builtin *builtin = builtin_lookup(env->builtins, node->args->value);
		if (builtin != NULL) {
			int stdin_copy = duplicate_fd(STDIN_FILENO);
			int stdout_copy = duplicate_fd(STDOUT_FILENO);
			in = duplicate_fd(in);
			out = duplicate_fd(out);
			resolve_redirections(node, &in, &out, true);
			redirect_fd(in, STDIN_FILENO);
			redirect_fd(out, STDOUT_FILENO);
			t_result result = result_create(true, builtin->block(env, node->args_size, node->args));
			redirect_fd(stdin_copy, STDIN_FILENO);
			redirect_fd(stdout_copy, STDOUT_FILENO);
			return result;
		}
	}

	int	pid = fork();

	if (pid == 0) {

		close_fd(to_be_closed);

		resolve_redirections(node, &in, &out, false);

		redirect_fd(in, STDIN_FILENO);
		redirect_fd(out, STDOUT_FILENO);

		if (node->args_size > 0) {
			char **args = unwrap_args(node->args, node->args_size);
			char **envp = unwrap_env(env->symbols, env->symbols_size);
			char *executable_path = resolve_executable(env, node->args[0].value);
			if (executable_path != NULL)
				execve(executable_path, args, envp);
			else
				errno = ENOENT;
			ft_printf(STDERR_FILENO, "minishell: %s: %s\n", args[0], strerror(errno));
			if (errno == ENOENT)
				exit(127);
			if (errno == EACCES)
				exit(126);
		}

		exit(255);
	}

	if (should_wait)
		return (result_create(true, await_process(pid)));
	return (result_create(true, pid));
}

t_result	visit_node_internal(t_environment *env, t_node *node, int in, int out, int to_be_closed, bool should_wait);

t_result	visit_parent_node(t_environment *env, t_parent_node *node, int in, int out, int to_be_closed, bool should_wait)
{
	return (visit_node_internal(env, node->expression, in, out, to_be_closed, should_wait));
}

t_result	visit_pipe_node(t_environment *env, t_pipe_node *node, int in, int out, int to_be_closed, bool should_wait)
{
	(void) to_be_closed;
	int read_write[2];
	pipe(read_write);
	// ft_printf(STDERR_FILENO, "pipe(read: %d, write: %d)\n", read_write[0], read_write[1]);
	t_result pid1 = visit_node_internal(env, node->left, in, read_write[1], read_write[0], false);
	t_result pid2 = visit_node_internal(env, node->right, read_write[0], out, read_write[1], false);
	close_fd(read_write[1]);
	close_fd(read_write[0]);
	if (pid1.success && should_wait)
		await_process(pid1.extra);
	if (pid1.success && should_wait)
		return (result_create(true, await_process(pid2.extra)));
	return (pid2);
}

t_result	visit_conjuction_node(t_environment *env, t_conjuction_node *node, int in, int out, int to_be_closed, bool should_wait)
{
	if (string_equals(node->operator->lexeme, "&&")) {
		t_result res = visit_node_internal(env, node->left, in, out, to_be_closed, should_wait);
		if (res.success && res.extra != 0)
			return (res);
		return (visit_node_internal(env, node->right, in, out, to_be_closed, should_wait));
	} else if (string_equals(node->operator->lexeme, "||")) {
		t_result res = visit_node_internal(env, node->left, in, out, to_be_closed, should_wait);
		if (res.success && res.extra == 0)
			return (res);
		return (visit_node_internal(env, node->right, in, out, to_be_closed, should_wait));
	} else {
		error("Error: Illegal state in 'visit_conjuction_node'");
		return (result_create(false, 0));
	}
}

t_result	visit_node_internal(t_environment *env, t_node *node, int in, int out, int to_be_closed, bool should_wait)
{
	if (node->type == COMMAND_NODE)
		return (visit_command_node(env, (t_command_node *) node, in, out, to_be_closed, should_wait));
	else if (node->type == PARENTHESES_NODE)
		return (visit_parent_node(env, (t_parent_node *) node, in, out, to_be_closed, should_wait));
	else if (node->type == PIPELINE_NODE)
		return (visit_pipe_node(env, (t_pipe_node *) node, in, out, to_be_closed, should_wait));
	else if (node->type == CONJUCTION_NODE)
		return (visit_conjuction_node(env, (t_conjuction_node *) node, in, out, to_be_closed, should_wait));
	else
	{
		error("Error: Illegal state in 'visit_node'");
		return (result_create(false, 0));
	}
}

t_result	visit_node(t_environment *env, t_node *node)
{
	return (visit_node_internal(env, node, -1, -1, -1, true));
}

// --------------------------------------------------------
