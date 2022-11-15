/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:20:00 by soubella          #+#    #+#             */
/*   Updated: 2022/11/14 20:13:04 by soubella         ###   ########.fr       */
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
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (WTERMSIG(status) + 128);
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
			if (*in != -1)
				close(*in);
			*in = open(redirect.extra.value, O_RDONLY);
			if (*in == -1)
				error("minishell: %s: %s", strerror(errno), redirect.extra.value);
		} else if (redirect.type == OUTPUT) {
			if (*out != -1)
				close(*out);
			*out = open(redirect.extra.value, O_WRONLY | O_TRUNC | O_CREAT, 0644);
			if (*out == -1)
				error("minishell: %s: %s", strerror(errno), redirect.extra.value);
		} else if (redirect.type == HEREDOC) {
			if (*in != -1)
				close(*in);
			int read_write[2];
			pipe(read_write);
			write(read_write[1], redirect.extra.value, string_length(redirect.extra.value));
			close(read_write[1]);
			*in = read_write[0];
		} else if (redirect.type == APPEND) {
			if (*out != -1)
				close(*out);
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

// TODO - REDIRECT FOR BUILTINS
t_result	visit_command_node(
	t_environment *env,
	t_command_node *node,
	int in, int out, int to_be_closed,
	bool should_wait
)
{
	if (node->args_size > 0)
	{
		if (string_equals(node->args->value, "echo"))
			return (resolve_redirections(node, &in, &out, true),
				result_create(true, echo_builtin(env, node->args_size, node->args)));
		if (string_equals(node->args->value, "cd"))
			return (resolve_redirections(node, &in, &out, true),
				result_create(true, cd_builtin(env, node->args_size, node->args)));
		if (string_equals(node->args->value, "pwd"))
			return (resolve_redirections(node, &in, &out, true),
				result_create(true, pwd_builtin(env, node->args_size, node->args)));
		if (string_equals(node->args->value, "export"))
			return (resolve_redirections(node, &in, &out, true),
				result_create(true, export_builtin(env, node->args_size, node->args)));
		if (string_equals(node->args->value, "unset"))
			return (resolve_redirections(node, &in, &out, true),
				result_create(true, unset_builtin(env, node->args_size, node->args)));
		if (string_equals(node->args->value, "exit"))
			return (resolve_redirections(node, &in, &out, true),
				result_create(true, exit_builtin(env, node->args_size, node->args)));
		if (string_equals(node->args->value, "env"))
			return (resolve_redirections(node, &in, &out, true),
				result_create(true, env_builtin(env, node->args_size, node->args)));
	}

	t_resolve_result exec;
	
	if (node->args_size > 0) {
		exec = resolve_executable(env, node->args[0].value);
		if (exec.err_type != NONE)
		{
			ft_printf(STDERR_FILENO, "minishell: %s: %s\n", node->args[0].value, error_msg(exec.err_type));
			if (exec.err_type == NOT_FOUND)
				return (result_create(false, 127));
			else if (exec.err_type == PERMISSION_DENIED)
				return (result_create(false, 126));
			error("Illegal state in 'visit_command_node'");
			return (result_create(false, 0));
		}
	}

	int	pid = fork();

	if (pid == 0) {

		if (to_be_closed != -1)
			close(to_be_closed);

		resolve_redirections(node, &in, &out, false);

		if (in != -1) {
			dup2(in, STDIN_FILENO);
			close(in);
		}

		if (out != -1) {
			dup2(out, STDOUT_FILENO);
			close(out);
		}
		
		if (node->args_size > 0) {
			char **args = unwrap_args(node->args, node->args_size);
			char **envp = unwrap_env(env->symbols, env->symbols_size);
			execve(exec.executable_path.string.value, args, envp);
			ft_printf(STDERR_FILENO, "minishell: %s: %s\n", args[0], strerror(errno));
		}
		
		exit(1);
	}
	if (exec.executable_path.present)
		string_free(&exec.executable_path.string);
	if (should_wait)
		return (result_create(true, await_process(pid)));
	return (result_create(true, pid));
}

t_result	visit_parent_node(t_environment *env, t_parent_node *node, int in, int out, int to_be_closed, bool should_wait)
{
	return (visit_node(env, node->expression, in, out, to_be_closed, should_wait));
}

t_result	visit_pipe_node(t_environment *env, t_pipe_node *node, int in, int out, int to_be_closed, bool should_wait)
{
	(void) to_be_closed;
	(void) should_wait;
	(void) env;
	(void) env;
	int read_write[2];
	pipe(read_write);
	t_result pid1 = visit_node(env, node->left, in, read_write[1], read_write[0], false);
	t_result pid2 = visit_node(env, node->right, read_write[0], out, read_write[1], false);
	close(read_write[0]);
	close(read_write[1]);
	if (pid1.success)
		await_process(pid1.extra);
	if (pid2.success)
		return (result_create(true, await_process(pid2.extra)));
	return (pid2);
}

t_result	visit_conjuction_node(t_environment *env, t_conjuction_node *node, int in, int out, int to_be_closed, bool should_wait)
{
	(void) node;
	(void) env;
	if (string_equals(node->operator->lexeme, "&&")) {
		t_result res = visit_node(env, node->left, in, out, to_be_closed, should_wait);
		if (res.success && res.extra != 0)
			return (res);
		return (visit_node(env, node->right, in, out, to_be_closed, should_wait));
	} else if (string_equals(node->operator->lexeme, "||")) {
		t_result res = visit_node(env, node->left, in, out, to_be_closed, should_wait);
		if (res.success && res.extra == 0)
			return (res);
		return (visit_node(env, node->right, in, out, to_be_closed, should_wait));
	} else {
		error("Error: Illegal state in 'visit_conjuction_node'");
		return (result_create(false, 0));
	}
}

t_result	visit_node(t_environment *env, t_node *node, int in, int out, int to_be_closed, bool should_wait)
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

// --------------------------------------------------------

void	initilize_defaults(t_environment *env)
{
	t_symbol		symbol;

	symbol.name = string_create("PWD", false);
	symbol.value = string_create(env->working_dir.value, false);
	env_put_var(env, symbol);
	symbol.name = string_create("SHLVL", false);
	symbol.value = string_create(int_to_string(string_to_int(env_get_var(env, "SHLVL", "")) + 1), true);
	env_put_var(env, symbol);
	symbol.name = string_create("_", false);
	symbol.value = string_create("/usr/bin/env", false);
	env_put_var(env, symbol);
}

t_environment *environment_new(char **env)
{
	t_environment	*environment;
	char			**splitted;
	t_symbol		symbol;

	environment = malloc(sizeof(t_environment));
	if (environment == NULL)
		memory_error();
	environment->symbols = NULL;
	environment->last_exit_code = 0;
	environment->symbols_size = 0;
	environment->symbols_cap = 0;
	environment->running = true;
	environment->working_dir = string_create(getcwd(NULL, 0), true);
	if (environment->working_dir.value == NULL)
		error("Error: Cannot retreive the current working directory");
	while (*env)
	{
		splitted = string_split(*env++, "=");
		symbol.name = string_create(splitted[0], splitted[0] != NULL);
		symbol.value = string_create(splitted[1], splitted[1] != NULL);
		env_put_var(environment, symbol);
		free(splitted);
	}
	initilize_defaults(environment);
	return (environment);
}

void	environment_free(t_environment **env)
{
	size_t	index;

	index = -1;
	while (++index < (*env)->symbols_size)
	{
		string_free(&(*env)->symbols[index].name);
		string_free(&(*env)->symbols[index].value);
	}
	string_free(&(*env)->working_dir);
	free((*env)->symbols);
	free(*env);
	*env = NULL;
}

void	env_insert_var(t_environment *env, t_symbol symbol)
{
	t_symbol	*new_symbols;
	size_t		new_capacity;

	if (env->symbols_size + 1 >= env->symbols_cap)
	{
		new_capacity = (env->symbols_size + 1) << 1;
		new_symbols = malloc(sizeof(t_symbol) * new_capacity);
		if (new_symbols == NULL)
			memory_error();
		memcpy(new_symbols, env->symbols, env->symbols_size * sizeof(t_symbol));
		free(env->symbols);
		env->symbols_cap = new_capacity;
		env->symbols = new_symbols;
	}
	env->symbols[env->symbols_size] = symbol;
	env->symbols_size++;
}

char	*env_get_var(t_environment *env, char *name, char *fallback)
{
	size_t	target;
	size_t	index;

	target = -1;
	index = -1;
	while (++index < env->symbols_size)
		if (string_equals(env->symbols[index].name.value, name))
			return (env->symbols[index].value.value);
	return (fallback);
}

void	env_remove_var(t_environment *env, char *name)
{
	ssize_t	target;
	size_t	index;

	target = -1;
	index = -1;
	if (string_equals(name, "_"))
		return ;
	while (++index < env->symbols_size)
	{
		if (!string_equals(env->symbols[index].name.value, name))
			continue ;
		target = index;
		break ;
	}
	if (target == -1)
		return ;
	memmove(&env->symbols[target], &env->symbols[target + 1], sizeof(t_symbol) * (env->symbols_size - target));
	env->symbols_size--;
}

void	env_put_var(t_environment *env, t_symbol symbol)
{
	size_t	index;

	index = -1;
	while (++index < env->symbols_size)
	{
		if (string_equals(env->symbols[index].name.value, symbol.name.value))
		{
			string_free(&env->symbols[index].value);
			env->symbols[index].value = symbol.value;
			return ;
		}
	}
	env_insert_var(env, symbol);
}
