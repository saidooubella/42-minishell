/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:14:08 by soubella          #+#    #+#             */
/*   Updated: 2022/11/30 21:33:31 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "ft_printf.h"
#include "parser.h"

t_optional_token	parser_expect(
	t_parser *parser, t_token_type type, char *expected)
{
	if (parser_current_is(parser, type))
		return (token_optional(parser_consume(parser), true));
	ft_printf(STDERR_FILENO, "Error: Expected '%s' but got '%s'\n",
		expected, parser_consume(parser)->lexeme);
	return (token_optional(NULL, false));
}

t_string	current_lexeme(t_parser *parser)
{
	return (string_create(parser_consume(parser)->lexeme, false));
}

t_elements	*word_expr(t_parser *parser, bool in_string)
{
	t_elements	*elements;

	elements = elements_new_cap(1);
	elements_add(elements, current_lexeme(parser), WORD_ELEMENT, false, in_string);
	return (elements);
}

t_elements	*var_expr(t_parser *parser, bool var_expantion, bool in_string)
{
	t_elements	*elements;
	t_token		*dollar;
	t_string	temp;

	dollar = parser_consume(parser);
	elements = elements_new_cap(1);
	if (var_expantion)
		elements_add(elements, current_lexeme(parser), VAR_ELEMENT, true, in_string);
	else
	{
		temp = string_create(string_join(dollar->lexeme,
					parser_consume(parser)->lexeme), true);
		elements_add(elements, temp, WORD_ELEMENT, false, in_string);
	}
	return (elements);
}

t_elements	*string_expr(t_parser *parser, bool var_expantion)
{
	t_elements	*elements;
	t_elements	*temp;

	parser_consume(parser);
	elements = elements_new();
	while (!parser_current_is(parser, CLOSE_DOUBLE_QUOTE))
	{
		if (parser_current_is(parser, WORD))
			elements_add(elements, current_lexeme(parser), WORD_ELEMENT, false, true);
		else if (!parser_current_is(parser, DOLLAR))
			error("Error: Illegal state in 'string_expr'");
		else
		{
			temp = var_expr(parser, var_expantion, true);
			elements_fill(elements, &temp);
		}
	}
	parser_expect(parser, CLOSE_DOUBLE_QUOTE, "\"");
	return (elements);
}
