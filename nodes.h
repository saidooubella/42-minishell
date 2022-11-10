/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 13:38:46 by soubella          #+#    #+#             */
/*   Updated: 2022/11/10 13:40:54 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NODES_H
# define NODES_H

# include "tokens.h"
# include "string.h"

typedef enum e_node_type
{
	CONJUCTION_NODE,
	PIPELINE_NODE,
	PARENTHESES_NODE,
	COMMAND_NODE,
}	t_node_type;

typedef enum e_redirection_type
{
	HEREDOC,
	APPEND,
	OUTPUT,
	INPUT,
}	t_redirection_type;

typedef struct s_node
{
	t_node_type	type;
}	t_node;

typedef struct s_redirection
{
	t_redirection_type	type;
	t_string			extra;
}	t_redirection;

typedef struct s_command_node
{
	t_node			header;
	t_redirection	*redirections;
	t_string		*args;
	size_t			redirections_size;
	size_t			redirections_cap;
	size_t			args_size;
	size_t			args_cap;
}	t_command_node;

typedef struct s_parent_node
{
	t_node	header;
	t_token	*left_parent;
	t_node	*expression;
	t_token	*right_parent;
}	t_parent_node;

typedef struct s_pipe_node
{
	t_node	header;
	t_node	*left;
	t_token	*operator;
	t_node	*right;
}	t_pipe_node;

typedef struct s_conjuction_node
{
	t_node	header;
	t_node	*left;
	t_token	*operator;
	t_node	*right;
}	t_conjuction_node;

t_node	*parent_node_new(
	t_token *left_parent, t_node *expression, t_token *right_parent);
t_node	*pipe_node_new(t_node *left, t_token *operator, t_node *right);
t_node	*conjuction_node_new(t_node *left, t_token *operator, t_node *right);
t_node	*command_node_new(void);
void	command_add_arg(t_node	*_node, t_string arg);
void	command_add_redirection(
	t_node	*_node, t_redirection_type type, t_string extra);

#endif
