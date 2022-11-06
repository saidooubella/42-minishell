/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/06 11:05:42 by soubella          #+#    #+#             */
/*   Updated: 2022/11/06 17:56:33 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"
#include "nodes.h"

t_node	*nodes_new(void)
{
	t_nodes	*nodes;

	nodes = malloc(sizeof(t_nodes));
	nodes->header.type = BATCH_NODE;
	if (nodes == NULL)
		memory_error();
	nodes->nodes = malloc(sizeof(t_node *) * 16);
	if (nodes->nodes == NULL)
		memory_error();
	nodes->capacity = 16;
	nodes->size = 0;
	return ((t_node *) nodes);
}

void	nodes_add(t_node *_nodes, t_node *node)
{
	size_t	new_capacity;
	t_node	**new_nodes;
	t_nodes	*nodes;

	nodes = (t_nodes *) _nodes;
	if (nodes->size + 1 >= nodes->capacity)
	{
		new_capacity = (nodes->size + 1) << 1;
		new_nodes = malloc(sizeof(t_node *) * new_capacity);
		if (new_nodes == NULL)
			memory_error();
		memcpy(new_nodes, nodes->nodes, nodes->size * sizeof(t_node *));
		free(nodes->nodes);
		nodes->capacity = new_capacity;
		nodes->nodes = new_nodes;
	}
	nodes->nodes[nodes->size] = node;
	nodes->size++;
}
