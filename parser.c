#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "string_builder.h"
#include "tokens.h"
#include "parser.h"
#include "utils.h"

t_string	string_create(char *value, bool freeable)
{
	t_string	string;

	string.freeable = freeable;
	string.value = value;
	return (string);
}

void	string_free(t_string *string)
{
	if (string->freeable)
		free(string->value);
	string->value = NULL;
}

t_node	*parent_node_new(t_token *left_parent, t_node *expression, t_token *right_parent)
{
	t_parent_node *node;

	node = malloc(sizeof(t_parent_node));
	if (node == NULL)
		memory_error();
	node->header.type = PARENTHESES_NODE;
	node->left_parent = left_parent;
	node->expression = expression;
	node->right_parent = right_parent;
	return ((t_node *) node);
}

t_node	*pipe_node_new(t_node *left, t_token *operator, t_node *right)
{
	t_pipe_node *node;

	node = malloc(sizeof(t_pipe_node));
	if (node == NULL)
		memory_error();
	node->header.type = PIPELINE_NODE;
	node->left = left;
	node->operator = operator;
	node->right = right;
	return ((t_node *) node);
}

t_node	*conjuction_node_new(t_node *left, t_token *operator, t_node *right)
{
	t_conjuction_node *node;

	node = malloc(sizeof(t_conjuction_node));
	if (node == NULL)
		memory_error();
	node->header.type = CONJUCTION_NODE;
	node->left = left;
	node->operator = operator;
	node->right = right;
	return ((t_node *) node);
}

t_node	*command_node_new()
{
	t_command_node *node;

	node = malloc(sizeof(t_command_node));
	if (node == NULL)
		memory_error();
	node->header.type = COMMAND_NODE;
	node->redirections = malloc(sizeof(t_redirection) * 16);
	if (node->redirections == NULL)
		memory_error();
	node->redirections_size = 0;
	node->redirections_cap = 16;
	node->args = malloc(sizeof(char *) * 16);
	if (node->args == NULL)
		memory_error();
	node->args_size = 0;
	node->args_cap = 16;
	return ((t_node *) node);
}

void	command_add_arg(t_node	*_node, t_string arg)
{
	t_command_node	*node;
	size_t			new_capacity;
	t_string		*new_args;

	node = (t_command_node *) _node;
	if (node->args_size + 1 >= node->args_cap)
	{
		new_capacity = (node->args_size + 1) << 1;
		new_args = malloc(sizeof(t_string) * new_capacity);
		if (new_args == NULL)
			memory_error();
		memcpy(new_args, node->args, node->args_size * sizeof(t_string));
		free(node->args);
		node->args_cap = new_capacity;
		node->args = new_args;
	}
	node->args[node->args_size].freeable = arg.freeable;
	node->args[node->args_size].value = arg.value;
	node->args_size++;
}

void	command_add_redirection(t_node	*_node, t_redirection_type type, t_string extra)
{
	t_command_node	*node;
	t_redirection	*new_redirects;
	size_t			new_capacity;

	node = (t_command_node *) _node;
	if (node->redirections_size + 1 >= node->redirections_cap)
	{
		new_capacity = (node->redirections_size + 1) << 1;
		new_redirects = malloc(sizeof(t_redirection) * new_capacity);
		if (new_redirects == NULL)
			memory_error();
		memcpy(new_redirects, node->redirections, node->redirections_size * sizeof(t_redirection));
		free(node->redirections);
		node->redirections_cap = new_capacity;
		node->redirections = new_redirects;
	}
	node->redirections[node->redirections_size].extra = extra;
	node->redirections[node->redirections_size].type = type;
	node->redirections_size++;
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

t_token	*parser_expect(t_parser *parser, t_token_type type, char *expected)
{
	if (parser_current_is(parser, type))
		return parser_consume(parser);
	error("Expected '%s' but got '%s", expected, parser_consume(parser)->lexeme);
	return (NULL);
}

char	*resolve_variable(char *name)
{
	return (name);
}

t_string	unit_expression(t_parser *parser)
{
	t_string_builder	*builder;
	char				*result;

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
			else if (parser_expect(parser, DOLLAR, "$"))
				string_builder_append_cstring(builder, resolve_variable(parser_consume(parser)->lexeme));
		}
		parser_expect(parser, CLOSE_DOUBLE_QUOTE, "\"");
		result = string_builder_to_cstr(builder);
		string_builder_free(&builder);
		return (string_create(result, true));
	}
	if (parser_current_is(parser, DOLLAR))
	{
		parser_consume(parser);
		return (string_create(resolve_variable(parser_consume(parser)->lexeme), false));
	}
	return (string_create(NULL, false));
}

t_string	apply_concatenation(t_parser *parser)
{
	t_string_builder	*builder;
	char				*result;
	t_string			temp;

	builder = string_builder_new();
	temp = unit_expression(parser);
	if (temp.value != NULL)
	{
		(string_builder_append_cstring(builder, temp.value), string_free(&temp));
		while (!parser_reached_end(parser) && parser_current_is(parser, PLUS))
		{
			parser_consume(parser);
			temp = unit_expression(parser);
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

t_string	redirection_operand(t_parser *parser)
{
	if (parser_current_is(parser, DOUBLE_GREATER_THAN)
		|| parser_current_is(parser, DOUBLE_AMPERSAND)
		|| parser_current_is(parser, DOUBLE_LESS_THAN)
		|| parser_current_is(parser, CLOSE_PARENT)
		|| parser_current_is(parser, GREATER_THAN)
		|| parser_current_is(parser, DOUBLE_PIPE)
		|| parser_current_is(parser, OPEN_PARENT)
		|| parser_current_is(parser, LESS_THAN))
		error("Unexpected '%s'", parser_consume(parser)->lexeme);
	return (apply_concatenation(parser));
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
			t_string	extra = redirection_operand(parser);
			command_add_redirection(command, APPEND, extra);
		}
		else if (parser_current_is(parser, DOUBLE_LESS_THAN))
		{
			parser_consume(parser);
			t_string	extra = redirection_operand(parser);
			command_add_redirection(command, HEREDOC, extra);
		}
		else if (parser_current_is(parser, GREATER_THAN))
		{
			parser_consume(parser);
			t_string	extra = redirection_operand(parser);
			command_add_redirection(command, OUTPUT, extra);
		}
		else if (parser_current_is(parser, LESS_THAN))
		{
			parser_consume(parser);
			t_string	extra = redirection_operand(parser);
			command_add_redirection(command, INPUT, extra);
		}
		else
		{
			t_string	arg = apply_concatenation(parser);
			if (arg.value == NULL)
				break ;
			command_add_arg(command, arg);
		}
	}
	if (((t_command_node *) command)->redirections_size == 0
		&& ((t_command_node *) command)->args_size == 0)
		error("Unexpected '%s'", parser_consume(parser)->lexeme);
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
