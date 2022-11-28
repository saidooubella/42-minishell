/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:06:39 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:06:58 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "parser.h"

t_node	*command_node_new(void)
{
	t_command_node	*node;

	node = malloc(sizeof(t_command_node));
	if (node == NULL)
		memory_error();
	node->header.type = COMMAND_NODE;
	node->redirections = malloc(sizeof(t_redirection) * 16);
	if (node->redirections == NULL)
		memory_error();
	node->redirections_size = 0;
	node->redirections_cap = 16;
	node->args = malloc(sizeof(t_string) * 16);
	if (node->args == NULL)
		memory_error();
	node->args_size = 0;
	node->args_cap = 16;
	return ((t_node *) node);
}

void	command_add_arg(t_node	*_node, t_elements *arg)
{
	t_command_node	*node;
	t_elements		**new_args;
	size_t			new_capacity;

	node = (t_command_node *) _node;
	if (node->args_size + 1 >= node->args_cap)
	{
		new_capacity = (node->args_size + 1) << 1;
		new_args = malloc(sizeof(t_elements *) * new_capacity);
		if (new_args == NULL)
			memory_error();
		bytes_copy(new_args, node->args,
			node->args_size * sizeof(t_elements *));
		free(node->args);
		node->args_cap = new_capacity;
		node->args = new_args;
	}
	node->args[node->args_size] = arg;
	node->args_size++;
}

void	command_add_redirection(
	t_node	*_node, t_redirection_type type, t_elements *extra)
{
	t_command_node	*node;
	t_redirection	*new_redirects;
	size_t			new_capacity;

	node = (t_command_node *) _node;
	if (node->redirections_size + 1 >= node->redirections_cap)
	{
		new_capacity = (node->redirections_size + 1) << 1;
		new_redirects = malloc(sizeof(t_redirection) * new_capacity);
		if (new_redirects == NULL)
			memory_error();
		bytes_copy(new_redirects, node->redirections,
			node->redirections_size * sizeof(t_redirection));
		free(node->redirections);
		node->redirections_cap = new_capacity;
		node->redirections = new_redirects;
	}
	node->redirections[node->redirections_size].elements = extra;
	node->redirections[node->redirections_size].type = type;
	node->redirections_size++;
}
