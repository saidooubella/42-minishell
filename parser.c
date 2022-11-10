#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "string_builder.h"
#include "interpreter.h"
#include "tokens.h"
#include "parser.h"
#include "utils.h"
#include "nodes.h"

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

t_token	*parser_expect(t_parser *parser, t_token_type type, char *expected)
{
	if (parser_current_is(parser, type))
		return parser_consume(parser);
	error("Error: Expected '%s' but got '%s", expected, parser_consume(parser)->lexeme);
	return (NULL);
}

t_string	unit_expression(t_parser *parser, bool expand)
{
	t_string_builder	*builder;
	t_token				*dollar;
	char				*result;

	(void) expand;
	if (parser_current_is(parser, WORD))
		return (string_create(parser_consume(parser)->lexeme, false));
	if (parser_current_is(parser, OPEN_DOUBLE_QUOTE))
	{
		builder = string_builder_new();
		parser_consume(parser);
		while (!parser_reached_end(parser) && !parser_current_is(parser, CLOSE_DOUBLE_QUOTE))
		{
			if (parser_current_is(parser, WORD))
				string_builder_append_cstring(builder, parser_consume(parser)->lexeme);
			else if (parser_current_is(parser, DOLLAR))
			{
				dollar = parser_consume(parser);
				if (expand)
					string_builder_append_cstring(builder, env_get_var(parser->env, parser_consume(parser)->lexeme, ""));
				else
				{
					string_builder_append_cstring(builder, dollar->lexeme);
					string_builder_append_cstring(builder, parser_consume(parser)->lexeme);
				}
			}
			else
				error("Error: Illegal state in 'unit_expression'");
		}
		parser_expect(parser, CLOSE_DOUBLE_QUOTE, "\"");
		result = string_builder_to_cstr(builder);
		string_builder_free(&builder);
		return (string_create(result, true));
	}
	if (parser_current_is(parser, DOLLAR))
	{
		dollar = parser_consume(parser);
		if (expand)
			return (string_create(env_get_var(parser->env, parser_consume(parser)->lexeme, ""), false));
		return (string_create(string_join(dollar->lexeme, parser_consume(parser)->lexeme), true));
	}
	return (string_create(NULL, false));
}

t_string	concatenation_expression(t_parser *parser, bool expand)
{
	t_string_builder	*builder;
	char				*result;
	t_string			temp;

	builder = string_builder_new();
	temp = unit_expression(parser, expand);
	if (temp.value != NULL)
	{
		(string_builder_append_cstring(builder, temp.value), string_free(&temp));
		while (!parser_reached_end(parser) && parser_current_is(parser, PLUS))
		{
			parser_consume(parser);
			temp = unit_expression(parser, expand);
			if (temp.value == NULL)
				break ;
			(string_builder_append_cstring(builder, temp.value), string_free(&temp));
		}
	}
	result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	return (string_create(result, true));
}

t_node	*conjuction_expression(t_parser *parser);

t_string	redirection_operand(t_parser *parser, bool expand)
{
	if (parser_current_is(parser, DOUBLE_GREATER_THAN)
		|| parser_current_is(parser, DOUBLE_AMPERSAND)
		|| parser_current_is(parser, DOUBLE_LESS_THAN)
		|| parser_current_is(parser, CLOSE_PARENT)
		|| parser_current_is(parser, GREATER_THAN)
		|| parser_current_is(parser, DOUBLE_PIPE)
		|| parser_current_is(parser, OPEN_PARENT)
		|| parser_current_is(parser, LESS_THAN))
		error("Error: Unexpected '%s', expected a word", parser_consume(parser)->lexeme);
	return (concatenation_expression(parser, expand));
}

t_node	*primary_expression(t_parser *parser)
{
	t_token	*left_parent;
	t_node	*expression;
	t_token	*right_parent;

	if (parser_current_is(parser, OPEN_PARENT))
	{
		left_parent = parser_consume(parser);
		expression = conjuction_expression(parser);
		right_parent = parser_expect(parser, CLOSE_PARENT, ")");
		return (parent_node_new(left_parent, expression, right_parent));
	}
	t_node	*command = command_node_new();
	while (!parser_reached_end(parser))
	{
		if (parser_current_is(parser, DOUBLE_GREATER_THAN))
		{
			parser_consume(parser);
			t_string	extra = redirection_operand(parser, true);
			command_add_redirection(command, APPEND, extra);
		}
		else if (parser_current_is(parser, DOUBLE_LESS_THAN))
		{
			parser_consume(parser);
			t_string	extra = redirection_operand(parser, false);
			command_add_redirection(command, HEREDOC, extra);
		}
		else if (parser_current_is(parser, GREATER_THAN))
		{
			parser_consume(parser);
			t_string	extra = redirection_operand(parser, true);
			command_add_redirection(command, OUTPUT, extra);
		}
		else if (parser_current_is(parser, LESS_THAN))
		{
			parser_consume(parser);
			t_string	extra = redirection_operand(parser, true);
			command_add_redirection(command, INPUT, extra);
		}
		else if (parser_current_is(parser, WORD)
				|| parser_current_is(parser, OPEN_DOUBLE_QUOTE)
				|| parser_current_is(parser, DOLLAR))
		{
			t_string	arg = concatenation_expression(parser, true);
			command_add_arg(command, arg);
		}
		else
			break ;
	}
	if (((t_command_node *) command)->redirections_size == 0
		&& ((t_command_node *) command)->args_size == 0)
		error("Error: Unexpected '%s'", parser_consume(parser)->lexeme);
	return (command);
}

t_node	*pipeline_expression(t_parser *parser)
{
	t_token	*operator;
	t_node	*left;

	left = primary_expression(parser);
	while (!parser_reached_end(parser) && parser_current_is(parser, PIPE))
	{
		operator = parser_consume(parser);
		left = pipe_node_new(left, operator, primary_expression(parser));
	}
	return (left);
}

t_node	*conjuction_expression(t_parser *parser)
{
	t_token	*operator;
	t_node	*left;

	left = pipeline_expression(parser);
	while (!parser_reached_end(parser) && (parser_current_is(parser, DOUBLE_AMPERSAND) || parser_current_is(parser, DOUBLE_PIPE)))
	{
		operator = parser_consume(parser);
		left = conjuction_node_new(left, operator, pipeline_expression(parser));
	}
	return (left);
}

t_node	*parse(t_parser *parser)
{
	return (conjuction_expression(parser));
}
