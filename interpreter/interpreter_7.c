/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_7.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 21:04:25 by soubella          #+#    #+#             */
/*   Updated: 2022/12/30 14:46:29 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>

#include "exec_resolver.h"
#include "string_utils.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "builtins.h"
#include "utils.h"

bool	has_multiple_parts(char *string)
{
	char	**splitted;
	bool	result;
	size_t	size;

	splitted = string_split(string, " \t\v\n\r\f");
	result = false;
	size = 0;
	while (splitted[size])
		size++;
	if (size != 1)
		result = true;
	size = -1;
	while (splitted[++size])
		free(splitted[size]);
	free(splitted);
	return (result);
}

t_result	forked_builtin(
	t_environment *env, t_builtin *builtin, char **args, size_t size)
{
	int	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_printf(STDOUT_FILENO, "minishell: Couldn't fork a child process\n");
		return (result_create(ERROR, 0));
	}
	if (pid == 0)
		exit(builtin->block(env, size, args));
	return (result_create(PID, pid));
}

bool	exec_builtin_prologue(
	t_environment *env, t_command_node *node,
	t_visit_extras *extra, t_exec_locals *locals)
{
	locals->args = unwrap_args(env, node, &locals->size, false);
	locals->stdin_copy = duplicate_fd(STDIN_FILENO);
	if (locals->stdin_copy == -2)
		return (false);
	locals->stdout_copy = duplicate_fd(STDOUT_FILENO);
	if (locals->stdout_copy == -2)
		return (false);
	extra->in = duplicate_fd(extra->in);
	if (extra->in == -2)
		return (false);
	extra->out = duplicate_fd(extra->out);
	if (extra->out == -2)
		return (false);
	return (true);
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

void	child_exec(
	t_environment *env, t_command_node *node,
	t_to_be_closed *tbc, t_visit_extras extra)
{
	size_t		index;

	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		error("Error: Failed to register a signal");
	index = -1;
	while (++index < tbc->size)
		close_fd(tbc->fds[index]);
	exec_program(env, node, extra);
}
