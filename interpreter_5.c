/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_5.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:36:24 by soubella          #+#    #+#             */
/*   Updated: 2022/11/27 11:34:13 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#include "environment.h"
#include "ft_printf.h"
#include "parser.h"
#include "utils.h"
#include "main.h"

t_result	visit_parent_node(
	t_environment *env, t_parent_node *node,
	t_to_be_closed *tbc, t_visit_extras extra)
{
	return (visit_node_internal(env, node->expression, tbc, extra));
}

t_result	visit_pipe_node(
	t_environment *env, t_pipe_node *node,
	t_to_be_closed *tbc, t_visit_extras extra)
{
	int			read_write[2];
	t_result	result;

	if (pipe(read_write) == -1)
		error("Failed to create a pipe");
	to_be_closed_add(tbc, read_write[0]);
	visit_node_internal(env, node->left, tbc,
		visit_extras(extra.in, read_write[1], false));
	tbc->size--;
	to_be_closed_add(tbc, read_write[1]);
	result = visit_node_internal(env, node->right, tbc,
			visit_extras(read_write[0], extra.out, false));
	tbc->size--;
	close_fd(read_write[0]);
	close_fd(read_write[1]);
	if (extra.should_wait)
	{
		result = await_process(result);
		while (wait(NULL) != -1)
			;
	}
	g_globals.in_pipe_node = true;
	return (result);
}

t_result	visit_conjuction_node(
	t_environment *env, t_conjuction_node *node,
	t_to_be_closed *tbc, t_visit_extras extra)
{
	t_result	res;

	if (string_equals(node->operator->lexeme, "&&"))
	{
		res = visit_node_internal(env, node->left, tbc,
				visit_extras(extra.in, extra.out, true));
		if (res.type == EXIT_STATUS && res.extra != 0)
			return (res);
		return (visit_node_internal(env, node->right, tbc, extra));
	}
	else if (string_equals(node->operator->lexeme, "||"))
	{
		res = visit_node_internal(env, node->left, tbc,
				visit_extras(extra.in, extra.out, true));
		if (res.type == EXIT_STATUS && res.extra == 0)
			return (res);
		return (visit_node_internal(env, node->right, tbc, extra));
	}
	error("Error: Illegal state in 'visit_conjuction_node'");
	return (result_create(ERROR, 0));
}

t_result	await_process(t_result result)
{
	int	status;

	if (result.type != PID)
		return (result);
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
