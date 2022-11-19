/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:20:00 by soubella          #+#    #+#             */
/*   Updated: 2022/11/19 09:58:51 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "string_builder.h"
#include "exec_resolver.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "builtins.h"
#include "parser.h"
#include "utils.h"
#include "nodes.h"

// --------------------------------------------------------

/*
	nested folders printing and pwd visiting places shouldn't visit
*/

t_result	result_create(t_result_type type, int extra)
{
	t_result	result;

	result.extra = extra;
	result.type = type;
	return (result);
}

char	**unwrap_args(t_environment *env, t_elements **args, size_t size)
{
	size_t	index;
	char	**mapped;
	
	mapped = malloc(sizeof(char *) * (size + 1));
	if (mapped == NULL)
		memory_error();
	index = -1;
	while (++index < size)
		mapped[index] = elements_resolve(args[index], env).value;
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

t_result	await_process(t_result result)
{
	int status;

	if (result.type != PID)
		return result;
	status = 0;
	waitpid(result.extra, &status, 0);
	if (WIFEXITED(status))
		return (result_create(EXIT_STATUS, WEXITSTATUS(status)));
	if (WTERMSIG(status) == SIGINT)
		ft_printf(STDOUT_FILENO, "\n");
	if (WTERMSIG(status) == SIGQUIT)
		ft_printf(STDOUT_FILENO, "Quit: 3\n");
	return (result_create(EXIT_STATUS, WTERMSIG(status) + 128));
}

void	close_fd(int fd)
{
	if (fd == -1)
		return ;
	close(fd);
}

int	duplicate_fd(int fd)
{
	int	new;

	if (fd != -1)
		new = dup(fd);
	else
		new = -1;
	return (new);
}

void	redirect_fd(int src, int dst) {
	if (src == -1)
		return ;
	dup2(src, dst);
	close_fd(src);
}

void	resolve_redirections(
	t_environment *env, t_command_node *node, int *in, int *out, bool builtin)
{
	size_t	index;

	(void) builtin;
	index = -1;
	while (++index < node->redirections_size)
	{
		t_redirection redirect = node->redirections[index];
		if (redirect.type == INPUT) {
			close_fd(*in);
			t_string path = elements_resolve(redirect.elements, env);
			*in = open(path.value, O_RDONLY);
			if (*in == -1)
				error("minishell: %s: %s", strerror(errno), path);
			string_free(&path);
		} else if (redirect.type == OUTPUT) {
			close_fd(*out);
			t_string path = elements_resolve(redirect.elements, env);
			*out = open(path.value, O_WRONLY | O_TRUNC | O_CREAT, 0644);
			if (*out == -1)
				error("minishell: %s: %s", strerror(errno), path);
			string_free(&path);
		} else if (redirect.type == HEREDOC) {
			close_fd(*in);
			int read_write[2];
			pipe(read_write);
			t_string content = elements_resolve(redirect.elements, env);
			write(read_write[1], content.value, string_length(content.value));
			close_fd(read_write[1]);
			*in = read_write[0];
			string_free(&content);
		} else if (redirect.type == APPEND) {
			close_fd(*out);
			t_string path = elements_resolve(redirect.elements, env);
			*out = open(path.value, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (*out == -1)
				error("minishell: %s: %s", strerror(errno), path.value);
			string_free(&path);
		}
	}
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
		t_string path = elements_resolve(node->args[0], env);
		t_builtin *builtin = builtin_lookup(env->builtins, path.value);
		string_free(&path);
		if (builtin != NULL) {
			int stdin_copy = duplicate_fd(STDIN_FILENO);
			int stdout_copy = duplicate_fd(STDOUT_FILENO);
			in = duplicate_fd(in);
			out = duplicate_fd(out);
			resolve_redirections(env, node, &in, &out, true);
			redirect_fd(in, STDIN_FILENO);
			redirect_fd(out, STDOUT_FILENO);
			t_result result = result_create(EXIT_STATUS, builtin->block(env, node->args_size, node->args));
			redirect_fd(stdin_copy, STDIN_FILENO);
			redirect_fd(stdout_copy, STDOUT_FILENO);
			return result;
		}
	}

	int	pid = fork();

	if (pid == 0) {

		close_fd(to_be_closed);
		
		resolve_redirections(env, node, &in, &out, false);

		redirect_fd(out, STDOUT_FILENO);
		redirect_fd(in, STDIN_FILENO);

		if (node->args_size > 0)
		{
			char **args = unwrap_args(env, node->args, node->args_size);
			char **envp = unwrap_env(env->symbols, env->symbols_size);
			char *executable_path = resolve_executable(env, args[0]);
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
		return (await_process(result_create(PID, pid)));
	return (result_create(PID, pid));
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
	visit_node_internal(env, node->left, in, read_write[1], read_write[0], false);
	t_result pid2 = visit_node_internal(env, node->right, read_write[0], out, read_write[1], false);
	close_fd(read_write[1]);
	close_fd(read_write[0]);
	if (should_wait)
	{
		pid2 = await_process(pid2);
		while (wait(NULL) != -1)
			;
	}
	return (pid2);
}

t_result	visit_conjuction_node(t_environment *env, t_conjuction_node *node, int in, int out, int to_be_closed, bool should_wait)
{
	if (string_equals(node->operator->lexeme, "&&")) {
		t_result res = visit_node_internal(env, node->left, in, out, to_be_closed, true);
		if (res.type == EXIT_STATUS && res.extra != 0)
			return (res);
		return (visit_node_internal(env, node->right, in, out, to_be_closed, should_wait));
	} else if (string_equals(node->operator->lexeme, "||")) {
		t_result res = visit_node_internal(env, node->left, in, out, to_be_closed, true);
		if (res.type == EXIT_STATUS && res.extra == 0)
			return (res);
		return (visit_node_internal(env, node->right, in, out, to_be_closed, should_wait));
	} else {
		error("Error: Illegal state in 'visit_conjuction_node'");
		return (result_create(ERROR, 0));
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
		return (result_create(ERROR, 0));
	}
}

t_result	visit_node(t_environment *env, t_node *node)
{
	t_result	result;

	result = visit_node_internal(env, node, -1, -1, -1, true);
	return (result);
}

// --------------------------------------------------------
