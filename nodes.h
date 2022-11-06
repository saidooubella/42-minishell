/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/06 11:05:41 by soubella          #+#    #+#             */
/*   Updated: 2022/11/06 18:38:53 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NODES_H
# define NODES_H

# include <stddef.h>

# include "parser.h"

typedef enum e_node_type
{
	OUT_REDIRECT_APPEND_NODE,
	PARENTHESIZED_NODE,
	OUT_REDIRECT_NODE,
	IN_REDIRECT_NODE,
	HEREDOC_IN_NODE,
	VARIABLE_NODE,
	CONCAT_NODE,
	BATCH_NODE,
	NOPE_NODE,
	WORD_NODE,
	PIPE_NODE,
	AND_NODE,
	OR_NODE,
}	t_node_type;

typedef struct s_node
{
	t_node_type	type;
}	t_node;

typedef struct s_nodes
{
	t_node	header;
	t_node	**nodes;
	size_t	capacity;
	size_t	size;
}	t_nodes;

void	nodes_add(t_node *nodes, t_node *node);
t_node	*nodes_new(void);

#endif
