/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:20:00 by soubella          #+#    #+#             */
/*   Updated: 2022/11/10 16:18:59 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "get_next_line.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "builtins.h"
#include "parser.h"
#include "utils.h"
#include "nodes.h"

// --------------------------------------------------------

int	read_from_stdin(char *limiter)
{
	int		read_write[2];
	char	*line;

	if (pipe(read_write) == -1)
	{
		perror("minishell");
		exit(0);
	}
	while (1)
	{
		ft_printf(STDERR_FILENO, "> ");
		line = get_next_line(STDIN_FILENO);
		if (line == 0)
			break ;
		if (are_equals(limiter, line))
		{
			free(line);
			break ;
		}
		write(read_write[1], line, string_length(line));
		free(line);
	}
	close(read_write[1]);
	return (read_write[0]);
}

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

int	visit_command_node(t_environment *env, t_command_node *node)
{
	if (string_equals(node->args->value, "echo"))
		return (echo_builtin(env, node->args_size, node->args));
	if (string_equals(node->args->value, "cd"))
		return (cd_builtin(env, node->args_size, node->args));
	if (string_equals(node->args->value, "pwd"))
		return (pwd_builtin(env, node->args_size, node->args));
	if (string_equals(node->args->value, "export"))
		return (export_builtin(env, node->args_size, node->args));
	if (string_equals(node->args->value, "unset"))
		return (unset_builtin(env, node->args_size, node->args));
	if (string_equals(node->args->value, "exit"))
		return (exit_builtin(env, node->args_size, node->args));
	if (string_equals(node->args->value, "env"))
		return (env_builtin(env, node->args_size, node->args));

	int	pid = fork();

	if (pid == 0) {

		size_t	index;
		int out = -1;
		int in = -1;

		index = -1;
		while (++index < node->redirections_size)
		{
			t_redirection redirect = node->redirections[index];
			if (redirect.type == INPUT) {
				if (in != -1) close(in);
				in = open(redirect.extra.value, O_RDONLY);
				if (in == -1)
					error("minishell: %s: %s\n", strerror(errno), redirect.extra.value);
			} else if (redirect.type == OUTPUT) {
				if (out != -1) close(out);
				out = open(redirect.extra.value, O_WRONLY | O_TRUNC | O_CREAT, 0644);
				if (in == -1)
					error("minishell: %s: %s\n", strerror(errno), redirect.extra.value);
			} else if (redirect.type == HEREDOC) {
				if (in != -1) close(in);
				in = read_from_stdin(redirect.extra.value);
			} else if (redirect.type == APPEND) {
				if (out != -1) close(out);
				out = open(redirect.extra.value, O_WRONLY | O_TRUNC | O_CREAT | O_APPEND, 0644);
				if (in == -1)
					error("minishell: %s: %s\n", strerror(errno), redirect.extra.value);
			}
		}

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
			execve(args[0], args, envp);
			free(args);
			index = -1;
			while (++index < env->symbols_size)
				free(envp[index]);
			free(envp);
			error("minishell: %s\n", strerror(errno));
		}
		
		exit(0);
	}
	int res;
	waitpid(pid, &res, 0);
	return (res);
}

int	visit_parent_node(t_environment *env, t_parent_node *node)
{
	return (visit_node(env, node->expression));
}

int	visit_pipe_node(t_environment *env, t_pipe_node *node)
{
	(void) node;
	(void) env;
	visit_node(env, node->left);
	visit_node(env, node->right);
	return (0);
}

int	visit_conjuction_node(t_environment *env, t_conjuction_node *node)
{
	(void) node;
	(void) env;
	visit_node(env, node->left);
	visit_node(env, node->right);
	return (0);
}

int	visit_node(t_environment *env, t_node *node)
{
	if (node->type == COMMAND_NODE)
		return (visit_command_node(env, (t_command_node *) node));
	else if (node->type == PARENTHESES_NODE)
		return (visit_parent_node(env, (t_parent_node *) node));
	else if (node->type == PIPELINE_NODE)
		return (visit_pipe_node(env, (t_pipe_node *) node));
	else if (node->type == CONJUCTION_NODE)
		return (visit_conjuction_node(env, (t_conjuction_node *) node));
	else
	{
		error("Error: Illegal state in 'visit_node'");
		return (0);
	}
}

// --------------------------------------------------------

t_environment *environment_new(char **env)
{
	t_environment	*environment;

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
		char **splitted = string_split(*env++, '=');
		t_symbol symbol;
		symbol.name = string_create(splitted[0], splitted[0] != NULL);
		symbol.value = string_create(splitted[1], splitted[1] != NULL);
		env_put_var(environment, symbol);
		free(splitted);
	}
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
	while (++index < env->symbols_size)
	{
		if (string_equals(env->symbols[index].name.value, name))
		{
			target = index;
			break ;
		}
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
/*

for (char *line; )

*/