/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:35:01 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 18:33:59 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "exec_resolver.h"
#include "entry_point.h"
#include "interpreter.h"
#include "ft_printf.h"

t_visit_extras	visit_extras(
	int in, int out, bool should_wait, bool enforce_fork)
{
	t_visit_extras	extras;

	extras.should_wait = should_wait;
	extras.enforce_fork = enforce_fork;
	extras.out = out;
	extras.in = in;
	return (extras);
}

static bool	exec_builtin_epilogue(char **args, int stdin_copy, int stdout_copy)
{
	char		**temp;

	if (!redirect_fd(stdin_copy, STDIN_FILENO))
		return (false);
	if (!redirect_fd(stdout_copy, STDOUT_FILENO))
		return (false);
	temp = args;
	while (*temp)
		free(*temp++);
	free(args);
	return (true);
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

	args = unwrap_args(env, node, &size, false);
	stdin_copy = duplicate_fd(STDIN_FILENO);
	if (stdin_copy == -2)
		return (result_create(ERROR, 0));
	stdout_copy = duplicate_fd(STDOUT_FILENO);
	if (stdout_copy == -2)
		return (result_create(ERROR, 0));
	extra.in = duplicate_fd(extra.in);
	if (extra.in == -2)
		return (result_create(ERROR, 0));
	extra.out = duplicate_fd(extra.out);
	if (extra.out == -2)
		return (result_create(ERROR, 0));
	if (resolve_program_io(env, node, extra.in, extra.out))
	{
		if (extra.enforce_fork)
			result = forked_builtin(env, builtin, args, size);
		else
			result = result_create(EXIT_STATUS,
					builtin->block(env, size, args));
	}
	else
		result = result_create(EXIT_STATUS, 1);
	if (extra.enforce_fork && extra.should_wait)
		result = await_process(result);
	if (!exec_builtin_epilogue(args, stdin_copy, stdout_copy))
		return (result_create(ERROR, false));
	return (result);
}

static void	exec_program(
	t_environment *env, t_command_node *node, t_visit_extras extra)
{
	char	**args;
	char	**envp;
	char	*executable_path;
	size_t	size;

	if (!resolve_program_io(env, node, extra.in, extra.out))
		exit(1);
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
	{
		ft_printf(STDOUT_FILENO, "minishell: Couldn't fork a child process\n");
		return (result_create(ERROR, 0));
	}
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
