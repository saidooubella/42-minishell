/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 13:39:15 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:08:09 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "nodes.h"
#include "utils.h"

t_optional_node	node_optional(t_node *node, bool present)
{
	t_optional_node	optional;

	optional.present = present;
	optional.node = node;
	return (optional);
}

t_node	*parent_node_new(
	t_token *left_parent, t_node *expression, t_token *right_parent)
{
	t_parent_node	*node;

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
	t_pipe_node	*node;

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
	t_conjuction_node	*node;

	node = malloc(sizeof(t_conjuction_node));
	if (node == NULL)
		memory_error();
	node->header.type = CONJUCTION_NODE;
	node->left = left;
	node->operator = operator;
	node->right = right;
	return ((t_node *) node);
}
