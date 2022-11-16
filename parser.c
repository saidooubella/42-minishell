/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:38:20 by soubella          #+#    #+#             */
/*   Updated: 2022/11/16 10:21:01 by soubella         ###   ########.fr       */
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
	parser->leftovers = tokens_new();
	parser->leftovers_head = 0;
	return (parser);
}

void	parser_free(t_parser **parser)
{
	tokens_free(&(*parser)->leftovers);
	free(*parser);
}

bool	parser_current_is(t_parser *parser, t_token_type type)
{
	return (parser->tokens->tokens[parser->index].type == type);
}

bool	parser_reached_end(t_parser *parser)
{
	return (parser->leftovers->size - parser->leftovers_head <= 0
		&& parser_current_is(parser, END_OF_FILE));
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

t_string	unit_expression(t_parser *parser, bool vars_expantion, bool *wildcard_expantion, bool *has_quotes)
{
	t_string_builder	*builder;
	t_token				*dollar;
	char				*result;

	if (parser->leftovers->size - parser->leftovers_head > 0)
	{
		t_token *leftover = &parser->leftovers->tokens[parser->leftovers_head++];
		return (string_create(leftover->lexeme, false));
	}
	if (parser_current_is(parser, WORD))
		return (string_create(parser_consume(parser)->lexeme, false));
	if (parser_current_is(parser, OPEN_DOUBLE_QUOTE))
	{
		*wildcard_expantion = false;
		*has_quotes = true;
		builder = string_builder_new();
		parser_consume(parser);
		while (!parser_reached_end(parser) && !parser_current_is(parser, CLOSE_DOUBLE_QUOTE))
		{
			if (parser_current_is(parser, WORD))
				string_builder_append_cstring(builder, parser_consume(parser)->lexeme);
			else if (parser_current_is(parser, DOLLAR))
			{
				dollar = parser_consume(parser);
				if (vars_expantion)
				{
					char *name = parser_consume(parser)->lexeme;
					if (string_equals(name, "?"))
					{
						char *num = llong_to_string(parser->env->exit_code);
						string_builder_append_cstring(builder, num);
						free(num);
					}
					else
						string_builder_append_cstring(builder, env_get_var(parser->env, name, ""));
				}
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
	if (parser_current_is(parser, OPEN_SINGLE_QUOTE))
	{
		*wildcard_expantion = false;
		*has_quotes = true;
		builder = string_builder_new();
		parser_consume(parser);
		if (parser_current_is(parser, WORD))
			string_builder_append_cstring(builder, parser_consume(parser)->lexeme);
		parser_expect(parser, CLOSE_SINGLE_QUOTE, "\'");
		result = string_builder_to_cstr(builder);
		string_builder_free(&builder);
		return (string_create(result, true));
	}
	if (parser_current_is(parser, DOLLAR))
	{
		*wildcard_expantion = false;
		dollar = parser_consume(parser);
		if (vars_expantion)
		{
			char *name = parser_consume(parser)->lexeme;
			if (string_equals(name, "?"))
				return (string_create(llong_to_string(parser->env->exit_code), true));
			char *value = env_get_var(parser->env, name, "");
			if (*value != '\0')
			{
				char **subs = string_split(value, " \t");
				char **temp = subs;
				char *ret = *temp++;
				while (*temp)
					tokens_add(parser->leftovers, *temp++, WORD);
				free(subs);
				return (string_create(ret, true));
			}
			return (string_create(value, false));
		}
		return (string_create(string_join(dollar->lexeme, parser_consume(parser)->lexeme), true));
	}
	return (string_create(NULL, false));
}

t_string	concatenation_expression(t_parser *parser, bool expand, bool *has_quotes)
{
	bool				wildcard_expantion;
	t_string_builder	*builder;
	char				*result;
	t_string			temp;

	builder = string_builder_new();
	wildcard_expantion = true;
	temp = unit_expression(parser, expand, &wildcard_expantion, has_quotes);
	if (temp.value != NULL)
	{
		(string_builder_append_cstring(builder, temp.value), string_free(&temp));
		while (!parser_reached_end(parser) && parser_current_is(parser, PLUS))
		{
			parser_consume(parser);
			temp = unit_expression(parser, expand, &wildcard_expantion, has_quotes);
			if (temp.value == NULL)
				break ;
			string_builder_append_cstring(builder, temp.value);
			string_free(&temp);
		}
	}
	result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	if (!wildcard_expantion)
		return (string_create(result, true));
	temp = apply_pattern(result, parser->env->working_dir.value);
	if (temp.value != result)
		free(result);
	return (temp);
}

t_optional_string	redirection_operand(t_parser *parser, bool expand, bool *has_quotes)
{
	if (parser_current_is(parser, DOUBLE_GREATER_THAN)
		|| parser_current_is(parser, DOUBLE_AMPERSAND)
		|| parser_current_is(parser, DOUBLE_LESS_THAN)
		|| parser_current_is(parser, CLOSE_PARENT)
		|| parser_current_is(parser, GREATER_THAN)
		|| parser_current_is(parser, DOUBLE_PIPE)
		|| parser_current_is(parser, OPEN_PARENT)
		|| parser_current_is(parser, LESS_THAN))
	{
		ft_printf(STDERR_FILENO, "Error: Unexpected '%s', expected a word\n", parser_consume(parser)->lexeme);
		return (string_optional(string_create(NULL, false), false));
	}
	return (string_optional(concatenation_expression(parser, expand, has_quotes), true));
}

t_string	resolve_vars(t_environment *env, char *line)
{
	if (string_index_of(line, "$") == -1)
		return (string_create(line, false));
	t_string_builder *builder = string_builder_new_cap(string_length(line));
	char *result;
	while (*line)
	{
		if (*line != '$' || (*line == '$' && !is_identifier_start(*(line + 1))))
		{
			string_builder_append_char(builder, *line++);
			continue ;
		}
		line++;
		size_t	index = 0;
		while (line[index] && is_identifier_cont(line[index])) {
			index++;
		}
		char *name = substring(line, 0, index);
		string_builder_append_cstring(builder, env_get_var(env, name, ""));
		free(name);
		line += index;
	}
	result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	return (string_create(result, true));
}

t_optional_string	read_from_stdin(t_environment *env, char *limiter, bool expand_vars)
{
	t_string_builder	*builder;
	char				*result;
	char				*line;
	bool				success;

	builder = string_builder_new();
	success = true;
	result = NULL;
	line = NULL;
	signal(SIGINT, sigint_close_handler);
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
			t_string resolved_line = resolve_vars(env, line);
			string_builder_append_cstring(builder, resolved_line.value);
			string_free(&resolved_line);
		}
		else
			string_builder_append_cstring(builder, line);
		string_builder_append_char(builder, '\n');
		free(line);
		line = NULL;
	}
	signal(SIGINT, sigint_handler);
	free(line);
	if (success)
		result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	return (string_optional(string_create(result, success), success));
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
			t_optional_string	extra = redirection_operand(parser, true, &has_quotes);
			if (!extra.present) return (node_optional(command, false));
			command_add_redirection(command, APPEND, extra.string);
		}
		else if (parser_current_is(parser, DOUBLE_LESS_THAN))
		{
			parser_consume(parser);
			t_optional_string	extra = redirection_operand(parser, false, &has_quotes);
			if (!extra.present) return (node_optional(command, false));
			t_optional_string	content = read_from_stdin(parser->env, extra.string.value, !has_quotes);
			string_free(&extra.string);
			if (!content.present) return (node_optional(command, false));
			command_add_redirection(command, HEREDOC, content.string);
		}
		else if (parser_current_is(parser, GREATER_THAN))
		{
			parser_consume(parser);
			t_optional_string	extra = redirection_operand(parser, true, &has_quotes);
			if (!extra.present) return (node_optional(command, false));
			command_add_redirection(command, OUTPUT, extra.string);
		}
		else if (parser_current_is(parser, LESS_THAN))
		{
			parser_consume(parser);
			t_optional_string	extra = redirection_operand(parser, true, &has_quotes);
			if (!extra.present) return (node_optional(command, false));
			command_add_redirection(command, INPUT, extra.string);
		}
		else if (parser_current_is(parser, WORD)
				|| parser_current_is(parser, OPEN_DOUBLE_QUOTE)
				|| parser_current_is(parser, OPEN_SINGLE_QUOTE)
				|| parser_current_is(parser, DOLLAR)
				|| parser->leftovers->size - parser->leftovers_head > 0)
			command_add_arg(command, concatenation_expression(parser, true, &has_quotes));
		else
			break ;
	}
	if (((t_command_node *) command)->redirections_size == 0
		&& ((t_command_node *) command)->args_size == 0)
		ft_printf(STDERR_FILENO, "Error: Unexpected '%s'\n", parser_consume(parser)->lexeme);
	return (node_optional(command, true));
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
