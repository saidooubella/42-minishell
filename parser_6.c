/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_6.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:20:38 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:20:39 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

t_optional_node	parent_expr(t_parser *parser)
{
	t_token				*left_parent;
	t_optional_node		expression;
	t_optional_token	right_parent;

	left_parent = parser_consume(parser);
	expression = conjuction_expression(parser);
	if (!expression.present)
		return (expression);
	right_parent = parser_expect(parser, CLOSE_PARENT, ")");
	if (!right_parent.present)
		return (node_optional(expression.node, false));
	return (node_optional(parent_node_new(left_parent,
				expression.node, right_parent.token), true));
}

bool	redirect_out_append_expr(t_parser *parser, t_node *command)
{
	t_optional_elements	extra;
	bool				has_quotes;

	has_quotes = false;
	parser_consume(parser);
	extra = redirection_operand(parser, true, &has_quotes);
	if (!extra.present)
		return (false);
	command_add_redirection(command, APPEND, extra.elements);
	return (true);
}

bool	heredoc_expr(t_parser *parser, t_node *command)
{
	t_optional_elements	extra;
	t_optional_elements	content;
	t_string			limiter;
	bool				expandable;
	bool				has_quotes;

	has_quotes = false;
	parser_consume(parser);
	extra = redirection_operand(parser, false, &has_quotes);
	if (!extra.present)
		return (false);
	expandable = extra.elements->expandable;
	limiter = elements_resolve(extra.elements, parser->env);
	elements_free(&extra.elements);
	content = read_from_stdin(limiter.value, !has_quotes);
	content.elements->expandable = expandable;
	command_add_redirection(command, HEREDOC, content.elements);
	string_free(&limiter);
	return (content.present);
}

bool	redirect_out_expr(t_parser *parser, t_node *command)
{
	t_optional_elements	extra;
	bool				has_quotes;

	has_quotes = false;
	parser_consume(parser);
	extra = redirection_operand(parser, true, &has_quotes);
	if (!extra.present)
		return (false);
	command_add_redirection(command, OUTPUT, extra.elements);
	return (true);
}
