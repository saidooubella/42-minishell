/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 11:33:32 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 13:42:25 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>

#include "environment.h"
#include "parser.h"

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
	{
		new = dup(fd);
		if (new == -1)
			error("Couldn't duplicate a file descriptor");
	}
	else
		new = -1;
	return (new);
}

void	redirect_fd(int src, int dst)
{
	if (src == -1)
		return ;
	close_fd(dst);
	if (dup2(src, dst) == -1)
		error("Couldn't redirect a file descriptor");
	close_fd(src);
}

t_result	visit_node_internal(
	t_environment *env, t_node *node, t_to_be_closed *tbc, t_visit_extras extra)
{
	if (node->type == COMMAND_NODE)
		return (visit_command_node(env, (t_command_node *) node, tbc, extra));
	else if (node->type == PARENTHESES_NODE)
		return (visit_parent_node(env, (t_parent_node *) node, tbc, extra));
	else if (node->type == PIPELINE_NODE)
		return (visit_pipe_node(env, (t_pipe_node *) node, tbc, extra));
	else if (node->type == CONJUCTION_NODE)
		return (visit_conjuction_node(env,
				(t_conjuction_node *) node, tbc, extra));
	else
	{
		error("Error: Illegal state in 'visit_node'");
		return (result_create(ERROR, 0));
	}
}

t_result	visit_node(t_environment *env, t_node *node)
{
	t_result		result;
	t_visit_extras	extras;
	t_to_be_closed	*tbc;

	tbc = to_be_closed_new();
	extras = visit_extras(-1, -1, true, false);
	result = visit_node_internal(env, node, tbc, extras);
	free(tbc->fds);
	free(tbc);
	return (result);
}
