/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/05 09:43:02 by soubella          #+#    #+#             */
/*   Updated: 2022/11/07 09:11:51 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "tokens.h"
# include "nodes.h"

typedef struct s_and_node
{
	t_node	header;
	t_node	*left;
	t_token	*operator;
	t_node	*right;
}	t_and_node;

typedef struct s_or_node
{
	t_node	header;
	t_node	*left;
	t_token	*operator;
	t_node	*right;
}	t_or_node;

typedef struct s_pipe_node
{
	t_node	header;
	t_node	*left;
	t_token	*operator;
	t_node	*right;
}	t_pipe_node;

typedef struct s_in_redirect_node
{
	t_node	header;
	t_token	*operator;
	t_node	*file_name;
	t_node	*operand;
}	t_in_redirect_node;

typedef struct s_out_redirect_node
{
	t_node	header;
	t_token	*operator;
	t_node	*file_name;
	t_node	*operand;
}	t_out_redirect_node;

typedef struct s_out_redirect_append_node
{
	t_node	header;
	t_token	*operator;
	t_node	*file_name;
	t_node	*operand;
}	t_out_redirect_append_node;

typedef struct s_heredoc_in_node
{
	t_node	header;
	t_token	*operator;
	t_node	*limiter;
	t_node	*operand;
}	t_heredoc_in_node;

typedef struct s_parenthesized_node
{
	t_node	header;
	t_token	*left_parent;
	t_node	*nodes;
	t_token	*right_parent;
}	t_parenthesized_node;

typedef struct s_concat_node
{
	t_node	header;
	t_node	*left;
	t_node	*right;
}	t_concat_node;

typedef struct s_word_node
{
	t_node	header;
	char	*word;
}	t_word_node;

typedef struct s_variable_node
{
	t_node	header;
	char	*name;
}	t_variable_node;

typedef struct s_parser
{
	t_tokens	*tokens;
	size_t		index;
}	t_parser;

typedef struct s_visitor_extra	t_visitor_extra;

typedef struct s_visitor
{
	void	(*visit_and_node)(
			t_visitor_extra *extra, t_and_node *node);
	void	(*visit_or_node)(
			t_visitor_extra *extra, t_or_node *node);
	void	(*visit_pipe_node)(
			t_visitor_extra *extra, t_pipe_node *node);
	void	(*visit_in_redirect_node)(
			t_visitor_extra *extra, t_in_redirect_node *node);
	void	(*visit_out_redirect_node)(
			t_visitor_extra *extra, t_out_redirect_node *node);
	void	(*visit_out_redirect_append_node)(
			t_visitor_extra *extra, t_out_redirect_append_node *node);
	void	(*visit_heredoc_in_node)(
			t_visitor_extra *extra, t_heredoc_in_node *node);
	void	(*visit_parenthesized_node)(
			t_visitor_extra *extra, t_parenthesized_node *node);
	void	(*visit_concat_node)(
			t_visitor_extra *extra, t_concat_node *node);
	void	(*visit_word_node)(
			t_visitor_extra *extra, t_word_node *node);
	void	(*visit_variable_node)(
			t_visitor_extra *extra, t_variable_node *node);
	void	(*visit_nodes)(
			t_visitor_extra *extra, t_nodes *node);
}	t_visitor;

typedef struct s_visitor_extra
{
	void		(*visit)(t_visitor_extra *extra, t_node *node);
	t_visitor	*visitor;
}	t_visitor_extra;

void		visit_node(t_visitor *visitor, t_node *node);
void		visitor_free(t_visitor **visitor);

t_parser	*parser_new(t_tokens *tokens);
void		parser_free(t_parser **parser);
t_node		*parse(t_parser *parser);

t_visitor	*interpreter_visitor(void);
t_visitor	*visualizer_visitor(void);
t_visitor	*freeing_visitor(void);

#endif
