/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:38:20 by soubella          #+#    #+#             */
/*   Updated: 2022/11/28 10:16:16 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "ft_printf.h"
#include "parser.h"

static t_optional_node	primary_expression(t_parser *parser)
{
	if (parser_current_is(parser, OPEN_PARENT))
		return (parent_expr(parser));
	return (command_expr(parser));
}

static t_optional_node	pipeline_expression(t_parser *parser)
{
	t_token			*operator;
	t_optional_node	right;
	t_optional_node	left;

	left = primary_expression(parser);
	if (!left.present)
		return (left);
	while (!parser_reached_end(parser) && parser_current_is(parser, PIPE))
	{
		operator = parser_consume(parser);
		right = primary_expression(parser);
		left.node = pipe_node_new(left.node, operator, right.node);
		if (!right.present)
			return (node_optional(left.node, false));
	}
	return (node_optional(left.node, true));
}

static t_optional_node	conjuction_expression(t_parser *parser)
{
	t_token			*operator;
	t_optional_node	right;
	t_optional_node	left;

	left = pipeline_expression(parser);
	if (!left.present)
		return (left);
	while (!parser_reached_end(parser)
		&& (parser_current_is(parser, DOUBLE_AMPERSAND)
			|| parser_current_is(parser, DOUBLE_PIPE)))
	{
		operator = parser_consume(parser);
		right = pipeline_expression(parser);
		left.node = conjuction_node_new(left.node, operator, right.node);
		if (!right.present)
			return (node_optional(left.node, false));
	}
	return (node_optional(left.node, true));
}

t_optional_node	expression(t_parser *parser)
{
	if (parser->env->is_bonus)
		return (conjuction_expression(parser));
	return (pipeline_expression(parser));
}

t_optional_node	parse(t_parser *parser)
{
	t_optional_node	root;

	root = expression(parser);
	if (root.present && parser->index < parser->tokens->size - 1)
		ft_printf(STDERR_FILENO,
			"Error: Unexpected '%s'\n", parser_consume(parser)->lexeme);
	return (node_optional(root.node,
			parser->index >= parser->tokens->size - 1));
}
