/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:05:06 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 15:35:05 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "free_aware_string.h"
#include "parser.h"

void	command_node_free(t_command_node *node)
{
	size_t	index;

	index = -1;
	while (++index < node->redirections_size)
		elements_free(&node->redirections[index].elements);
	index = -1;
	while (++index < node->args_size)
		elements_free(&node->args[index]);
	free(node->redirections);
	free(node->args);
	free(node);
}

void	parent_node_free(t_parent_node *node)
{
	node_free(node->expression);
	free(node);
}

void	pipe_node_free(t_pipe_node *node)
{
	node_free(node->right);
	node_free(node->left);
	free(node);
}

void	conjuction_node_free(t_conjuction_node *node)
{
	node_free(node->right);
	node_free(node->left);
	free(node);
}

void	node_free(t_node *node)
{
	if (node == NULL)
		return ;
	if (node->type == COMMAND_NODE)
		command_node_free((t_command_node *) node);
	else if (node->type == PARENTHESES_NODE)
		parent_node_free((t_parent_node *) node);
	else if (node->type == PIPELINE_NODE)
		pipe_node_free((t_pipe_node *) node);
	else if (node->type == CONJUCTION_NODE)
		conjuction_node_free((t_conjuction_node *) node);
	else
		error("Error: Illegal state in 'node_free'\n");
}
