/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 13:39:15 by soubella          #+#    #+#             */
/*   Updated: 2022/11/10 13:43:18 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "tokens.h"
#include "string.h"
#include "nodes.h"

t_node	*parent_node_new(
	t_token *left_parent, t_node *expression, t_token *right_parent)
{
	t_parent_node *node;

	node = malloc(sizeof(t_parent_node));
	if (node == NULL)
		memory_error();
	node->header.type = PARENTHESES_NODE;
	node->left_parent = left_parent;
	node->expression = expression;
	node->right_parent = right_parent;
	return ((t_node *) node);
}

t_node	*pipe_node_new(t_node *left, t_token *operator, t_node *right)
{
	t_pipe_node *node;

	node = malloc(sizeof(t_pipe_node));
	if (node == NULL)
		memory_error();
	node->header.type = PIPELINE_NODE;
	node->left = left;
	node->operator = operator;
	node->right = right;
	return ((t_node *) node);
}

t_node	*conjuction_node_new(t_node *left, t_token *operator, t_node *right)
{
	t_conjuction_node *node;

	node = malloc(sizeof(t_conjuction_node));
	if (node == NULL)
		memory_error();
	node->header.type = CONJUCTION_NODE;
	node->left = left;
	node->operator = operator;
	node->right = right;
	return ((t_node *) node);
}

t_node	*command_node_new(void)
{
	t_command_node *node;

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

void	command_add_arg(t_node	*_node, t_string arg)
{
	t_command_node	*node;
	t_string		*new_args;
	size_t			new_capacity;

	node = (t_command_node *) _node;
	if (node->args_size + 1 >= node->args_cap)
	{
		new_capacity = (node->args_size + 1) << 1;
		new_args = malloc(sizeof(t_string) * new_capacity);
		if (new_args == NULL)
			memory_error();
		memcpy(new_args, node->args, node->args_size * sizeof(t_string));
		free(node->args);
		node->args_cap = new_capacity;
		node->args = new_args;
	}
	node->args[node->args_size] = arg;
	node->args_size++;
}

void	command_add_redirection(
	t_node	*_node, t_redirection_type type, t_string extra)
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
		memcpy(new_redirects, node->redirections, node->redirections_size * sizeof(t_redirection));
		free(node->redirections);
		node->redirections_cap = new_capacity;
		node->redirections = new_redirects;
	}
	node->redirections[node->redirections_size].extra = extra;
	node->redirections[node->redirections_size].type = type;
	node->redirections_size++;
}
