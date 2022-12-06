/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_7.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:21:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/30 11:03:14 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "ft_printf.h"
#include "parser.h"

bool	redirect_in_expr(t_parser *parser, t_node *command)
{
	t_optional_elements	extra;
	bool				has_quotes;

	has_quotes = false;
	parser_consume(parser);
	extra = redirection_operand(parser, true, &has_quotes);
	if (!extra.present)
		return (false);
	command_add_redirection(command, INPUT, extra.elements);
	return (true);
}

bool	is_unit_expr_beg(t_parser *parser)
{
	return (parser_current_is(parser, WORD)
		|| parser_current_is(parser, OPEN_DOUBLE_QUOTE)
		|| parser_current_is(parser, OPEN_SINGLE_QUOTE)
		|| parser_current_is(parser, DOLLAR));
}

t_optional_node	validate_command(t_parser *parser, t_node *command)
{
	bool	present;

	present = true;
	if (((t_command_node *) command)->redirections_size == 0
		&& ((t_command_node *) command)->args_size == 0)
	{
		ft_printf(STDERR_FILENO,
			"Error: Unexpected '%s'\n", parser_consume(parser)->lexeme);
		present = false;
	}
	return (node_optional(command, present));
}

int	command_expr_unit(t_parser *parser, t_node *command)
{
	bool				has_quotes;

	has_quotes = false;
	if (parser_current_is(parser, DOUBLE_GREATER_THAN))
		return (redirect_out_append_expr(parser, command));
	if (parser_current_is(parser, DOUBLE_LESS_THAN))
		return (heredoc_expr(parser, command));
	if (parser_current_is(parser, GREATER_THAN))
		return (redirect_out_expr(parser, command));
	if (parser_current_is(parser, LESS_THAN))
		return (redirect_in_expr(parser, command));
	if (is_unit_expr_beg(parser))
	{
		command_add_arg(command,
			concatenation_expression(parser, true, &has_quotes));
		return (1);
	}
	return (-1);
}

t_optional_node	command_expr(t_parser *parser)
{
	t_node	*command;
	int		result;

	command = command_node_new();
	while (!parser_reached_end(parser))
	{
		result = command_expr_unit(parser, command);
		if (result == 0)
			return (node_optional(command, false));
		if (result == -1)
			break ;
	}
	return (validate_command(parser, command));
}
