/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:35:01 by soubella          #+#    #+#             */
/*   Updated: 2022/11/27 11:33:45 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "exec_resolver.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "main.h"

t_visit_extras	visit_extras(int in, int out, bool should_wait)
{
	t_visit_extras	extras;

	extras.should_wait = should_wait;
	extras.out = out;
	extras.in = in;
	return (extras);
}

static void	exec_builtin_epilogue(char **args, int stdin_copy, int stdout_copy)
{
	char		**tmp;

	redirect_fd(stdin_copy, STDIN_FILENO);
	redirect_fd(stdout_copy, STDOUT_FILENO);
	tmp = args;
	while (*tmp)
		free(*tmp++);
	free(args);
}

static t_result	exec_builtin(
	t_environment *env, t_command_node *node,
	t_visit_extras extra, t_builtin *builtin)
{
	int			stdout_copy;
	int			stdin_copy;
	char		**args;
	size_t		size;
	t_result	result;

	args = unwrap_args(env, node, &size,
			!string_equals(builtin->name, "export"));
	stdin_copy = duplicate_fd(STDIN_FILENO);
	stdout_copy = duplicate_fd(STDOUT_FILENO);
	extra.in = duplicate_fd(extra.in);
	extra.out = duplicate_fd(extra.out);
	resolve_redirections(env, node, &extra.in, &extra.out);
	redirect_fd(extra.in, STDIN_FILENO);
	redirect_fd(extra.out, STDOUT_FILENO);
	result = result_create(EXIT_STATUS,
			builtin->block(env, size, args));
	exec_builtin_epilogue(args, stdin_copy, stdout_copy);
	return (result);
}

static void	exec_program(
	t_environment *env, t_command_node *node, t_visit_extras extra)
{
	char	**args;
	char	**envp;
	char	*executable_path;
	size_t	size;

	resolve_redirections(env, node, &extra.in, &extra.out);
	redirect_fd(extra.out, STDOUT_FILENO);
	redirect_fd(extra.in, STDIN_FILENO);
	if (node->args_size > 0)
	{
		args = unwrap_args(env, node, &size, true);
		envp = unwrap_env(env->symbols, env->symbols_size);
		executable_path = resolve_executable(env, args[0]);
		if (executable_path != NULL)
			execve(executable_path, args, envp);
		else
			errno = ENOENT;
		ft_printf(2, "minishell: %s: %s\n", args[0], strerror(errno));
		if (errno == ENOENT)
			exit(127);
		if (errno == EACCES)
			exit(126);
	}
	exit(255);
}

t_result	visit_command_node(
	t_environment *env, t_command_node *node,
	t_to_be_closed *tbc, t_visit_extras extra)
{
	t_builtin	*builtin;
	size_t		index;
	int			pid;

	if (node->args_size > 0)
	{
		builtin = builtin_lookup(env, node->args[0]);
		if (builtin != NULL)
			return (exec_builtin(env, node, extra, builtin));
	}
	pid = fork();
	if (pid == -1)
		error("Couldn't fork a child process");
	if (pid == 0)
	{
		if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
			error("Error: Failed to register a signal");
		index = -1;
		while (++index < tbc->size)
			close_fd(tbc->fds[index]);
		exec_program(env, node, extra);
	}
	if (extra.should_wait)
		return (await_process(result_create(PID, pid)));
	return (result_create(PID, pid));
}
