/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:38:20 by soubella          #+#    #+#             */
/*   Updated: 2022/11/19 14:36:07 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "readline/readline.h"
#include "wildcard_matcher.h"
#include "string_builder.h"
#include "interpreter.h"
#include "lexer_utils.h"
#include "ft_printf.h"
#include "element.h"
#include "tokens.h"
#include "parser.h"
#include "utils.h"
#include "nodes.h"
#include "main.h"

t_parser	*parser_new(t_tokens *tokens, t_environment *env)
{
	t_parser	*parser;

	parser = malloc(sizeof(t_parser));
	if (parser == NULL)
		memory_error();
	parser->index = 0;
	parser->tokens = tokens;
	parser->env = env;
	return (parser);
}

void	parser_free(t_parser **parser)
{
	free(*parser);
}

bool	parser_current_is(t_parser *parser, t_token_type type)
{
	return (parser->tokens->tokens[parser->index].type == type);
}

bool	parser_reached_end(t_parser *parser)
{
	return (parser_current_is(parser, END_OF_FILE));
}

t_token	*parser_consume(t_parser *parser)
{
	return (&parser->tokens->tokens[parser->index++]);
}

t_optional_token	parser_expect(t_parser *parser, t_token_type type, char *expected)
{
	if (parser_current_is(parser, type))
		return (token_optional(parser_consume(parser), true));
	ft_printf(STDERR_FILENO, "Error: Expected '%s' but got '%s'\n", expected, parser_consume(parser)->lexeme);
	return (token_optional(NULL, false));
}

t_optional_elements	unit_expression(t_parser *parser, bool vars_expantion, bool *wildcard_expantion, bool *has_quotes)
{
	t_token	*dollar;

	if (parser_current_is(parser, WORD))
	{
		t_elements *elements = elements_new_cap(1);
		elements_add(elements, string_create(parser_consume(parser)->lexeme, false), WORD_ELEMENT, false);
		return (elements_optional(elements, true));
	}
	if (parser_current_is(parser, OPEN_DOUBLE_QUOTE))
	{
		t_elements *elements = elements_new();
		*wildcard_expantion = false;
		*has_quotes = true;
		parser_consume(parser);
		while (!parser_reached_end(parser) && !parser_current_is(parser, CLOSE_DOUBLE_QUOTE))
		{
			if (parser_current_is(parser, WORD))
				elements_add(elements, string_create(parser_consume(parser)->lexeme, false), WORD_ELEMENT, false);
			else if (parser_current_is(parser, DOLLAR))
			{
				dollar = parser_consume(parser);
				if (vars_expantion)
					elements_add(elements, string_create(parser_consume(parser)->lexeme, false), VAR_ELEMENT, true);
				else
					elements_add(elements, string_create(string_join(dollar->lexeme, parser_consume(parser)->lexeme), true), WORD_ELEMENT, false);
			}
			else
				error("Error: Illegal state in 'unit_expression'");
		}
		parser_expect(parser, CLOSE_DOUBLE_QUOTE, "\"");
		return (elements_optional(elements, true));
	}
	if (parser_current_is(parser, OPEN_SINGLE_QUOTE))
	{
		*wildcard_expantion = false;
		*has_quotes = true;
		t_elements *elements;
		parser_consume(parser);
		if (parser_current_is(parser, WORD))
		{
			elements = elements_new_cap(1);
			elements_add(elements, string_create(parser_consume(parser)->lexeme, false), WORD_ELEMENT, false);
		}
		else
			elements = elements_new_cap(0);
		parser_expect(parser, CLOSE_SINGLE_QUOTE, "\'");
		return (elements_optional(elements, true));
	}
	if (parser_current_is(parser, DOLLAR))
	{
		*wildcard_expantion = false;
		dollar = parser_consume(parser);
		t_elements *elements = elements_new_cap(1);
		if (vars_expantion)
			elements_add(elements, string_create(parser_consume(parser)->lexeme, false), VAR_ELEMENT, true);
		else
			elements_add(elements, string_create(string_join(dollar->lexeme, parser_consume(parser)->lexeme), true), WORD_ELEMENT, false);
		return (elements_optional(elements, true));
	}
	return (elements_optional(NULL, false));
}

t_elements	*concatenation_expression(t_parser *parser, bool expand, bool *has_quotes)
{
	bool				wildcard_expantion;
	t_elements			*elements;
	t_optional_elements temp;

	elements = elements_new();
	wildcard_expantion = true;
	temp = unit_expression(parser, expand, &wildcard_expantion, has_quotes);
	if (temp.elements != NULL)
	{
		elements_fill(elements, &temp.elements);
		while (!parser_reached_end(parser) && parser_current_is(parser, PLUS))
		{
			parser_consume(parser);
			temp = unit_expression(parser, expand, &wildcard_expantion, has_quotes);
			if (temp.elements == NULL)
				break ;
			elements_fill(elements, &temp.elements);
		}
	}
	elements->expandable = wildcard_expantion;
	return (elements);
}

t_optional_elements	redirection_operand(t_parser *parser, bool expand, bool *has_quotes)
{
	if (parser_reached_end(parser)
		|| parser_current_is(parser, DOUBLE_GREATER_THAN)
		|| parser_current_is(parser, DOUBLE_AMPERSAND)
		|| parser_current_is(parser, DOUBLE_LESS_THAN)
		|| parser_current_is(parser, CLOSE_PARENT)
		|| parser_current_is(parser, GREATER_THAN)
		|| parser_current_is(parser, DOUBLE_PIPE)
		|| parser_current_is(parser, OPEN_PARENT)
		|| parser_current_is(parser, LESS_THAN))
	{
		ft_printf(STDERR_FILENO, "Error: Unexpected '%s'\n", parser_consume(parser)->lexeme);
		return (elements_optional(NULL, false));
	}
	return (elements_optional(concatenation_expression(parser, expand, has_quotes), true));
}

t_elements	*resolve_vars(char *line)
{
	if (string_index_of(line, "$") == -1)
	{
		t_elements *elements = elements_new_cap(1);
		elements_add(elements, string_create(string_duplicate(line), true), WORD_ELEMENT, false);
		return (elements);
	}
	t_elements *elements = elements_new();
	t_string_builder *builder = string_builder_new_cap(string_length(line));
	while (*line)
	{
		if (*line != '$' || (*line == '$' && !is_identifier_start(*(line + 1))))
		{
			string_builder_append_char(builder, *line++);
			continue ;
		}
		if (builder->size > 0)
		{
			elements_add(elements, string_create(string_builder_to_cstr(builder), true), WORD_ELEMENT, true);
			string_builder_clear(builder);
		}
		line++;
		size_t	index = 0;
		while (line[index] && is_identifier_cont(line[index]))
			index++;
		elements_add(elements, string_create(substring(line, 0, index), true), VAR_ELEMENT, true);
		line += index;
	}
	if (builder->size > 0)
	{
		elements_add(elements, string_create(string_builder_to_cstr(builder), true), WORD_ELEMENT, false);
		string_builder_clear(builder);
	}
	string_builder_free(&builder);
	return (elements);
}

t_optional_elements	read_from_stdin(char *limiter, bool expand_vars)
{
	char	*line;
	bool	success;

	success = true;
	line = NULL;
	signal(SIGINT, sigint_close_handler);
	t_elements *elements = elements_new();
	while (1)
	{
		g_globals.in_fd = -1;
		line = readline("> ");
		if (g_globals.in_fd != -1 || line == NULL)
		{
			if (g_globals.in_fd != -1)
			{
				dup2(g_globals.in_fd, STDIN_FILENO);
				close(g_globals.in_fd);
				g_globals.in_fd = -1;
				success = false;
			}
			break ;
		}
		if (string_equals(limiter, line))
			break ;
		if (expand_vars)
		{
			char *str = string_join(line, "\n");
			t_elements *src = resolve_vars(str);
			elements_fill(elements, &src);
			free(str);
		}
		else
			elements_add(elements, string_create(string_join(line, "\n"), true), WORD_ELEMENT, false);
		free(line);
		line = NULL;
	}
	signal(SIGINT, sigint_handler);
	free(line);
	return (elements_optional(elements, success));
}

t_optional_node	conjuction_expression(t_parser *parser);

t_optional_node	primary_expression(t_parser *parser)
{
	t_token				*left_parent;
	t_optional_node		expression;
	t_optional_token	right_parent;

	if (parser_current_is(parser, OPEN_PARENT))
	{
		left_parent = parser_consume(parser);
		expression = conjuction_expression(parser);
		if (!expression.present)
			return (expression);
		right_parent = parser_expect(parser, CLOSE_PARENT, ")");
		if (!right_parent.present)
			return (node_optional(expression.node, false));
		return (node_optional(parent_node_new(left_parent, expression.node, right_parent.token), true));
	}
	t_node	*command = command_node_new();
	while (!parser_reached_end(parser))
	{
		bool	has_quotes = false;
		if (parser_current_is(parser, DOUBLE_GREATER_THAN))
		{
			parser_consume(parser);
			t_optional_elements	extra = redirection_operand(parser, true, &has_quotes);
			if (!extra.present) return (node_optional(command, false));
			command_add_redirection(command, APPEND, extra.elements);
		}
		else if (parser_current_is(parser, DOUBLE_LESS_THAN))
		{
			parser_consume(parser);
			t_optional_elements	extra = redirection_operand(parser, false, &has_quotes);
			if (!extra.present) return (node_optional(command, false));
			
			bool 		expandable = extra.elements->expandable;
			t_string	limiter = elements_resolve(extra.elements, parser->env);
			elements_free(&extra.elements);
			
			t_optional_elements	content = read_from_stdin(limiter.value, !has_quotes);
			content.elements->expandable = expandable;
			command_add_redirection(command, HEREDOC, content.elements);
			string_free(&limiter);
			
			if (!content.present) return (node_optional(command, false));
		}
		else if (parser_current_is(parser, GREATER_THAN))
		{
			parser_consume(parser);
			t_optional_elements	extra = redirection_operand(parser, true, &has_quotes);
			if (!extra.present) return (node_optional(command, false));
			command_add_redirection(command, OUTPUT, extra.elements);
		}
		else if (parser_current_is(parser, LESS_THAN))
		{
			parser_consume(parser);
			t_optional_elements	extra = redirection_operand(parser, true, &has_quotes);
			if (!extra.present) return (node_optional(command, false));
			command_add_redirection(command, INPUT, extra.elements);
		}
		else if (parser_current_is(parser, WORD)
				|| parser_current_is(parser, OPEN_DOUBLE_QUOTE)
				|| parser_current_is(parser, OPEN_SINGLE_QUOTE)
				|| parser_current_is(parser, DOLLAR))
			command_add_arg(command, concatenation_expression(parser, true, &has_quotes));
		else
			break ;
	}
	bool present = true;
	if (((t_command_node *) command)->redirections_size == 0
		&& ((t_command_node *) command)->args_size == 0)
	{
		ft_printf(STDERR_FILENO, "Error: Unexpected '%s'\n", parser_consume(parser)->lexeme);
		present = false;
	}
	return (node_optional(command, present));
}

t_optional_node	pipeline_expression(t_parser *parser)
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

t_optional_node	conjuction_expression(t_parser *parser)
{
	t_token			*operator;
	t_optional_node	right;
	t_optional_node	left;

	left = pipeline_expression(parser);
	if (!left.present)
		return (left);
	while (!parser_reached_end(parser) && (parser_current_is(parser, DOUBLE_AMPERSAND) || parser_current_is(parser, DOUBLE_PIPE)))
	{
		operator = parser_consume(parser);
		right = pipeline_expression(parser);
		left.node = conjuction_node_new(left.node, operator, right.node);
		if (!right.present)
			return (node_optional(left.node, false));
	}
	return (node_optional(left.node, true));
}

t_optional_node	parse(t_parser *parser)
{
	t_optional_node	root;

	root = conjuction_expression(parser);
	if (root.present && parser->index < parser->tokens->size - 1)
		ft_printf(STDERR_FILENO, "Error: Unexpected '%s'\n", parser_consume(parser)->lexeme);
	return (node_optional(root.node, parser->index >= parser->tokens->size - 1));
}
