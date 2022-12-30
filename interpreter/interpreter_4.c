/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:35:01 by soubella          #+#    #+#             */
/*   Updated: 2022/12/30 14:46:26 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

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
	t_exec_locals	locals;

	if (!exec_builtin_prologue(env, node, &extra, &locals))
		return (result_create(ERROR, 0));
	if (resolve_program_io(env, node, extra.in, extra.out))
	{
		if (extra.enforce_fork)
			locals.result = forked_builtin(env,
					builtin, locals.args, locals.size);
		else
			locals.result = result_create(EXIT_STATUS,
					builtin->block(env, locals.size, locals.args));
	}
	else
		locals.result = result_create(EXIT_STATUS, 1);
	if (extra.enforce_fork && extra.should_wait)
		locals.result = await_process(locals.result);
	if (!exec_builtin_epilogue(locals.args,
			locals.stdin_copy, locals.stdout_copy))
		return (result_create(ERROR, false));
	return (locals.result);
}

t_result	visit_command_node(
	t_environment *env, t_command_node *node,
	t_to_be_closed *tbc, t_visit_extras extra)
{
	t_builtin	*builtin;
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
		child_exec(env, node, tbc, extra);
	if (extra.should_wait)
		return (await_process(result_create(PID, pid)));
	return (result_create(PID, pid));
}
