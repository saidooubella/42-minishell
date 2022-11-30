/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_4.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:16:17 by soubella          #+#    #+#             */
/*   Updated: 2022/11/30 21:37:28 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "string_builder.h"
#include "lexer_utils.h"
#include "ft_printf.h"
#include "parser.h"

static t_elements	*raw_string_expr(t_parser *parser)
{
	t_elements	*elements;

	parser_consume(parser);
	if (parser_current_is(parser, WORD))
	{
		elements = elements_new_cap(1);
		elements_add(elements, current_lexeme(parser), WORD_ELEMENT, false, true);
	}
	else
		elements = elements_new_cap(0);
	parser_expect(parser, CLOSE_SINGLE_QUOTE, "\'");
	return (elements);
}

t_optional_elements	unit_expression(
	t_parser *parser, bool var_expantion,
	bool *wildcard_expantion, bool *has_quotes)
{
	if (parser_current_is(parser, WORD))
		return (elements_optional(word_expr(parser, false), true));
	if (parser_current_is(parser, OPEN_DOUBLE_QUOTE))
	{
		*wildcard_expantion = false;
		*has_quotes = true;
		return (elements_optional(string_expr(parser, var_expantion), true));
	}
	if (parser_current_is(parser, OPEN_SINGLE_QUOTE))
	{
		*wildcard_expantion = false;
		*has_quotes = true;
		return (elements_optional(raw_string_expr(parser), true));
	}
	if (parser_current_is(parser, DOLLAR))
	{
		*wildcard_expantion = false;
		return (elements_optional(var_expr(parser, var_expantion, false), true));
	}
	return (elements_optional(NULL, false));
}

t_elements	*concatenation_expression(
	t_parser *parser, bool expand, bool *has_quotes)
{
	bool				wildcard_expantion;
	t_elements			*elements;
	t_optional_elements	temp;

	elements = elements_new();
	wildcard_expantion = true;
	temp = unit_expression(parser, expand, &wildcard_expantion, has_quotes);
	if (temp.elements != NULL)
	{
		elements_fill(elements, &temp.elements);
		while (parser_current_is(parser, PLUS))
		{
			parser_consume(parser);
			temp = unit_expression(parser, expand,
					&wildcard_expantion, has_quotes);
			if (!temp.present)
				break ;
			elements_fill(elements, &temp.elements);
		}
	}
	elements->expandable = wildcard_expantion;
	return (elements);
}

t_optional_elements	redirection_operand(
	t_parser *parser, bool expand, bool *has_quotes)
{
	if (parser_current_is(parser, OPEN_DOUBLE_QUOTE)
		|| parser_current_is(parser, OPEN_SINGLE_QUOTE)
		|| parser_current_is(parser, VARIABLE)
		|| parser_current_is(parser, DOLLAR)
		|| parser_current_is(parser, WORD))
		return (elements_optional(
				concatenation_expression(parser, expand, has_quotes), true));
	ft_printf(STDERR_FILENO,
		"Error: Unexpected '%s'\n", parser_consume(parser)->lexeme);
	return (elements_optional(NULL, false));
}

char	*create_var(t_elements *elements, char *line, t_string_builder *builder)
{
	size_t	index;

	if (builder->size > 0)
	{
		elements_add(elements,
			string_create(string_builder_to_cstr(builder), true),
			WORD_ELEMENT, false, true);
		string_builder_clear(builder);
	}
	if (*line == '\0')
		return (line);
	index = 0;
	while (line[index] && is_identifier_cont(line[index]))
		index++;
	elements_add(elements,
		string_create(substring(line, 0, index), true),
		VAR_ELEMENT, true, true);
	return (line + index);
}
