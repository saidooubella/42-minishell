/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 11:27:36 by soubella          #+#    #+#             */
/*   Updated: 2022/11/07 09:54:24 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>

#include "tokens.h"
#include "utils.h"
#include "parser.h"
#include "nodes.h"

t_parser	*parser_new(t_tokens *tokens)
{
	t_parser	*parser;

	parser = malloc(sizeof(t_parser));
	if (parser == NULL)
		memory_error();
	parser->tokens = tokens;
	parser->index = 0;
	return (parser);
}

void	parser_free(t_parser **parser)
{
	free(*parser);
	*parser = NULL;
}

bool	parser_is(t_parser *parser, t_token_type type)
{
	return (parser->tokens->tokens[parser->index].type == type);
}

bool	parser_reached_end(t_parser *parser)
{
	return (parser->index >= parser->tokens->size
		&& parser_is(parser, END_OF_FILE));
}

t_token	*parser_current(t_parser *parser)
{
	return (&parser->tokens->tokens[parser->index]);
}

t_token	*parser_consume(t_parser *parser)
{
	t_token	*current;

	current = parser_current(parser);
	parser->index++;
	return (current);
}

// ----------------------------------------------------

t_node	*and_node_new(t_node *left, t_token *operator, t_node *right)
{
	t_and_node	*node;

	node = malloc(sizeof(t_and_node));
	if (node == NULL)
		memory_error();
	node->header.type = AND_NODE;
	node->operator = operator;
	node->right = right;
	node->left = left;
	return ((t_node *) node);
}

t_node	*or_node_new(t_node *left, t_token *operator, t_node *right)
{
	t_or_node	*node;

	node = malloc(sizeof(t_or_node));
	if (node == NULL)
		memory_error();
	node->header.type = OR_NODE;
	node->operator = operator;
	node->right = right;
	node->left = left;
	return ((t_node *) node);
}

t_node	*pipe_node_new(t_node *left, t_token *operator, t_node *right)
{
	t_pipe_node	*node;

	node = malloc(sizeof(t_or_node));
	if (node == NULL)
		memory_error();
	node->header.type = PIPE_NODE;
	node->operator = operator;
	node->right = right;
	node->left = left;
	return ((t_node *) node);
}

t_node	*in_redirect_node_new(
	t_token *operator, t_node *file_name, t_node *operand)
{
	t_in_redirect_node	*node;

	node = malloc(sizeof(t_in_redirect_node));
	if (node == NULL)
		memory_error();
	node->header.type = IN_REDIRECT_NODE;
	node->file_name = file_name;
	node->operator = operator;
	node->operand = operand;
	return ((t_node *) node);
}

t_node	*out_redirect_node_new(
	t_token *operator, t_node *file_name, t_node *operand)
{
	t_out_redirect_node	*node;

	node = malloc(sizeof(t_out_redirect_node));
	if (node == NULL)
		memory_error();
	node->header.type = OUT_REDIRECT_NODE;
	node->file_name = file_name;
	node->operator = operator;
	node->operand = operand;
	return ((t_node *) node);
}

t_node	*out_redirect_append_node_new(
	t_token *operator, t_node *file_name, t_node *operand)
{
	t_out_redirect_append_node	*node;

	node = malloc(sizeof(t_out_redirect_append_node));
	if (node == NULL)
		memory_error();
	node->header.type = OUT_REDIRECT_APPEND_NODE;
	node->file_name = file_name;
	node->operator = operator;
	node->operand = operand;
	return ((t_node *) node);
}

t_node	*heredoc_in_node_new(
	t_token *operator, t_node *limiter, t_node *operand)
{
	t_heredoc_in_node	*node;

	node = malloc(sizeof(t_heredoc_in_node));
	if (node == NULL)
		memory_error();
	node->header.type = HEREDOC_IN_NODE;
	node->operator = operator;
	node->limiter = limiter;
	node->operand = operand;
	return ((t_node *) node);
}

t_node	*parenthesized_node_new(
	t_token *left_parent, t_node *nodes, t_token *right_parent)
{
	t_parenthesized_node	*node;

	node = malloc(sizeof(t_parenthesized_node));
	if (node == NULL)
		memory_error();
	node->header.type = PARENTHESIZED_NODE;
	node->left_parent = left_parent;
	node->nodes = nodes;
	node->right_parent = right_parent;
	return ((t_node *) node);
}

t_node	*word_node_new(char *word)
{
	t_word_node	*node;

	node = malloc(sizeof(t_word_node));
	if (node == NULL)
		memory_error();
	node->header.type = WORD_NODE;
	node->word = word;
	return ((t_node *) node);
}

t_node	*concat_node_new(t_node *left, t_node *right)
{
	t_concat_node	*node;

	node = malloc(sizeof(t_concat_node));
	if (node == NULL)
		memory_error();
	node->header.type = CONCAT_NODE;
	node->right = right;
	node->left = left;
	return ((t_node *) node);
}

t_node	*variable_node_new(char *name)
{
	t_variable_node	*node;

	node = malloc(sizeof(t_variable_node));
	if (node == NULL)
		memory_error();
	node->header.type = VARIABLE_NODE;
	node->name = name;
	return ((t_node *) node);
}

// ----------------------------------------------------

t_node	*and_or_expression(t_parser *parser);

t_node	*parent_expression(t_parser *parser)
{
	t_node	*nodes;
	t_token	*right;
	t_token	*left;

	left = parser_consume(parser);
	nodes = nodes_new();
	while (!parser_is(parser, CLOSE_PARENT))
		nodes_add(nodes, and_or_expression(parser));
	if (parser_is(parser, CLOSE_PARENT))
	{
		right = parser_consume(parser);
		return (parenthesized_node_new(left, nodes, right));
	}
	error("Expected ')' but got '%s'", parser_current(parser)->lexeme);
	return (NULL);
}

void	build_string_nodes(t_parser *parser, t_node **node)
{
	t_node	*new;

	if (parser_is(parser, DOLLAR))
	{
		parser->index++;
		new = variable_node_new(parser_consume(parser)->lexeme);
	}
	else if (parser_is(parser, WORD))
		new = word_node_new(parser_consume(parser)->lexeme);
	else
		error("Internal error in 'build_string_nodes' %d", parser_current(parser)->type);
	if (*node == NULL)
		*node = new;
	else
		*node = concat_node_new(*node, new);
}

t_node	*string_expression(t_parser *parser)
{
	t_node	*node;

	parser->index += 1;
	node = NULL;
	while (!parser_reached_end(parser) && !parser_is(parser, C_DOUBLE_QUOTE))
		build_string_nodes(parser, &node);
	if (node == NULL)
		node = word_node_new("");
	if (parser_is(parser, C_DOUBLE_QUOTE))
	{
		parser->index++;
		return (node);
	}
	error("Expected '\"' but got '%s'", parser_current(parser)->lexeme);
	return (NULL);
}

t_node	*text_expression(t_parser *parser)
{
	if (parser_is(parser, WORD))
		return (word_node_new(parser_consume(parser)->lexeme));
	else if (parser_is(parser, O_DOUBLE_QUOTE))
		return (string_expression(parser));
	else if (parser_is(parser, DOLLAR))
	{
		parser->index++;
		return (variable_node_new(parser_consume(parser)->lexeme));
	}
	error("Unexpected '%s', expected expression",
		parser_current(parser)->lexeme);
	return (NULL);
}

t_node	*concat_expression(t_parser *parser)
{
	t_node	*left;

	left = text_expression(parser);
	while (!parser_reached_end(parser) && (parser_is(parser, PLUS)))
	{
		parser->index++;
		left = concat_node_new(left, text_expression(parser));
	}
	return (left);
}

t_node	*nodes_expression(t_parser *parser)
{
	t_node	*nodes;

	nodes = nodes_new();
	while (!parser_reached_end(parser) && (parser_is(parser, O_DOUBLE_QUOTE)
			|| parser_is(parser, DOLLAR) || parser_is(parser, WORD)))
		nodes_add(nodes, concat_expression(parser));
	return (nodes);
}

t_node	*primary_expression(t_parser *parser, bool optional)
{
	t_node	*nodes;

	if (parser_is(parser, OPEN_PARENT))
		return (parent_expression(parser));
	nodes = nodes_expression(parser);
	if (optional || ((t_nodes *) nodes)->size > 0)
		return (nodes);
	error("Unexpected '%s', expected expression",
		parser_current(parser)->lexeme);
	return (NULL);
}

bool	is_redirection_operator(t_parser *parser)
{
	return (parser_is(parser, DOUBLE_LESS_THAN)
		|| parser_is(parser, DOUBLE_GREATER_THAN)
		|| parser_is(parser, GREATER_THAN)
		|| parser_is(parser, LESS_THAN));
}

t_node	*redirection_operand(t_parser *parser)
{
	if (parser_is(parser, DOUBLE_GREATER_THAN)
		|| parser_is(parser, DOUBLE_AMPERSAND)
		|| parser_is(parser, DOUBLE_LESS_THAN)
		|| parser_is(parser, CLOSE_PARENT)
		|| parser_is(parser, GREATER_THAN)
		|| parser_is(parser, DOUBLE_PIPE)
		|| parser_is(parser, OPEN_PARENT)
		|| parser_is(parser, LESS_THAN))
		error("Unexpected '%s'", parser_current(parser)->lexeme);
	return (concat_expression(parser));
}

t_node	*suffix_expression(t_parser *parser)
{
	t_token	*operator;
	t_node	*file_name;
	t_node	*operand;

	operand = primary_expression(parser, is_redirection_operator(parser)
			|| parser_is(parser, END_OF_FILE));
	while (is_redirection_operator(parser))
	{
		operator = parser_consume(parser);
		file_name = redirection_operand(parser);
		if (operator->type == DOUBLE_LESS_THAN)
			operand = (heredoc_in_node_new(operator, file_name, operand));
		else if (operator->type == DOUBLE_GREATER_THAN)
			operand = (out_redirect_append_node_new(
						operator, file_name, operand));
		else if (operator->type == GREATER_THAN)
			operand = (out_redirect_node_new(operator, file_name, operand));
		else if (operator->type == LESS_THAN)
			operand = (in_redirect_node_new(operator, file_name, operand));
		else
			error("Internal error in 'suffix_expression'");
	}
	return (operand);
}

t_node	*prefix_expression(t_parser *parser)
{
	t_token	*operator;
	t_node	*file_name;
	t_node	*operand;

	if (is_redirection_operator(parser))
	{
		operator = parser_consume(parser);
		file_name = redirection_operand(parser);
		operand = prefix_expression(parser);
		if (operator->type == DOUBLE_LESS_THAN)
			return (heredoc_in_node_new(operator, file_name, operand));
		if (operator->type == DOUBLE_GREATER_THAN)
			return (out_redirect_append_node_new(operator, file_name, operand));
		if (operator->type == GREATER_THAN)
			return (out_redirect_node_new(operator, file_name, operand));
		if (operator->type == LESS_THAN)
			return (in_redirect_node_new(operator, file_name, operand));
		else
			error("Internal error in 'prefix_expression'");
	}
	return (suffix_expression(parser));
}

t_node	*pipe_expression(t_parser *parser)
{
	t_token	*operator;
	t_node	*left;

	left = prefix_expression(parser);
	while (!parser_reached_end(parser) && (parser_is(parser, PIPE)))
	{
		operator = parser_consume(parser);
		left = pipe_node_new(left, operator,
				prefix_expression(parser));
	}
	return (left);
}

t_node	*and_or_expression(t_parser *parser)
{
	t_token	*operator;
	t_node	*left;

	left = pipe_expression(parser);
	while (!parser_reached_end(parser) && (parser_is(parser, DOUBLE_AMPERSAND)
			|| parser_is(parser, DOUBLE_PIPE)))
	{
		operator = parser_consume(parser);
		if (operator->type == DOUBLE_AMPERSAND)
			left = and_node_new(left, operator,
					pipe_expression(parser));
		else
			left = or_node_new(left, operator,
					pipe_expression(parser));
	}
	return (left);
}

t_node	*parse(t_parser *parser)
{
	return (and_or_expression(parser));
}

// -----------------------------------------------------

bool	visit_node2(t_visitor_extra *extra, t_node *node)
{
	if (node->type == OUT_REDIRECT_APPEND_NODE)
	{
		extra->visitor->visit_out_redirect_append_node(
			extra, (t_out_redirect_append_node *) node);
		return (true);
	}
	if (node->type == PARENTHESIZED_NODE)
	{
		extra->visitor->visit_parenthesized_node(
			extra, (t_parenthesized_node *) node);
		return (true);
	}
	if (node->type == OUT_REDIRECT_NODE)
	{
		extra->visitor->visit_out_redirect_node(
			extra, (t_out_redirect_node *) node);
		return (true);
	}
	if (node->type == BATCH_NODE)
	{
		extra->visitor->visit_nodes(
			extra, (t_nodes *) node);
		return (true);
	}
	return (false);
}

void	visit_node1(t_visitor_extra *extra, t_node *node)
{
	if (visit_node2(extra, node))
		return ;
	if (node->type == IN_REDIRECT_NODE)
		extra->visitor->visit_in_redirect_node(
			extra, (t_in_redirect_node *) node);
	else if (node->type == HEREDOC_IN_NODE)
		extra->visitor->visit_heredoc_in_node(
			extra, (t_heredoc_in_node *) node);
	else if (node->type == VARIABLE_NODE)
		extra->visitor->visit_variable_node(
			extra, (t_variable_node *) node);
	else if (node->type == CONCAT_NODE)
		extra->visitor->visit_concat_node(extra, (t_concat_node *) node);
	else if (node->type == WORD_NODE)
		extra->visitor->visit_word_node(extra, (t_word_node *) node);
	else if (node->type == PIPE_NODE)
		extra->visitor->visit_pipe_node(extra, (t_pipe_node *) node);
	else if (node->type == AND_NODE)
		extra->visitor->visit_and_node(extra, (t_and_node *) node);
	else if (node->type == OR_NODE)
		extra->visitor->visit_or_node(extra, (t_or_node *) node);
	else
		error("Internal error in 'visit_node'");
}

void	visit_node(t_visitor *visitor, t_node *node)
{
	t_visitor_extra	extra;

	extra.visit = visit_node1;
	extra.visitor = visitor;
	visit_node1(&extra, node);
}

// -----------------------------------------------------

void	free_and_node(
	t_visitor_extra *extra, t_and_node *node)
{
	extra->visit(extra, node->right);
	extra->visit(extra, node->left);
	free(node);
}

void	free_or_node(
	t_visitor_extra *extra, t_or_node *node)
{
	extra->visit(extra, node->right);
	extra->visit(extra, node->left);
	free(node);
}

void	free_pipe_node(
	t_visitor_extra *extra, t_pipe_node *node)
{
	extra->visit(extra, node->right);
	extra->visit(extra, node->left);
	free(node);
}

void	free_in_redirect_node(
	t_visitor_extra *extra, t_in_redirect_node *node)
{
	extra->visit(extra, node->file_name);
	extra->visit(extra, node->operand);
	free(node);
}

void	free_out_redirect_node(
	t_visitor_extra *extra, t_out_redirect_node *node)
{
	extra->visit(extra, node->file_name);
	extra->visit(extra, node->operand);
	free(node);
}

void	free_out_redirect_append_node(
	t_visitor_extra *extra, t_out_redirect_append_node *node)
{
	extra->visit(extra, node->file_name);
	extra->visit(extra, node->operand);
	free(node);
}

void	free_heredoc_in_node(
	t_visitor_extra *extra, t_heredoc_in_node *node)
{
	extra->visit(extra, node->limiter);
	extra->visit(extra, node->operand);
	free(node);
}

void	free_parenthesized_node(
	t_visitor_extra *extra, t_parenthesized_node *node)
{
	extra->visit(extra, node->nodes);
	free(node);
}

void	free_concat_node(
	t_visitor_extra *extra, t_concat_node *node)
{
	extra->visit(extra, node->right);
	extra->visit(extra, node->left);
	free(node);
}

void	free_word_node(
	t_visitor_extra *extra, t_word_node *node)
{
	(void) extra;
	free(node);
}

void	free_variable_node(
	t_visitor_extra *extra, t_variable_node *node)
{
	(void) extra;
	free(node);
}

void	free_nodes(
	t_visitor_extra *extra, t_nodes *node)
{
	size_t	index;

	index = -1;
	while (++index < node->size)
		extra->visit(extra, node->nodes[index]);
	free(node->nodes);
	free(node);
}

t_visitor	*freeing_visitor(void)
{
	t_visitor	*visitor;

	visitor = malloc(sizeof(t_visitor));
	if (visitor == NULL)
		memory_error();
	visitor->visit_and_node = free_and_node;
	visitor->visit_or_node = free_or_node;
	visitor->visit_pipe_node = free_pipe_node;
	visitor->visit_in_redirect_node = free_in_redirect_node;
	visitor->visit_out_redirect_node = free_out_redirect_node;
	visitor->visit_out_redirect_append_node = free_out_redirect_append_node;
	visitor->visit_heredoc_in_node = free_heredoc_in_node;
	visitor->visit_parenthesized_node = free_parenthesized_node;
	visitor->visit_concat_node = free_concat_node;
	visitor->visit_word_node = free_word_node;
	visitor->visit_variable_node = free_variable_node;
	visitor->visit_nodes = free_nodes;
	return (visitor);
}

void	visitor_free(t_visitor **visitor)
{
	free(*visitor);
	*visitor = NULL;
}

// ---------------------------------------------
/*
void	visit_and_node(
	t_visitor_extra *extra, t_and_node *node)
{
}

void	visit_or_node(
	t_visitor_extra *extra, t_or_node *node)
{
}

void	visit_pipe_node(
	t_visitor_extra *extra, t_pipe_node *node)
{
}

void	visit_in_redirect_node(
	t_visitor_extra *extra, t_in_redirect_node *node)
{
}

void	visit_out_redirect_node(
	t_visitor_extra *extra, t_out_redirect_node *node)
{
}

void	visit_out_redirect_append_node(
	t_visitor_extra *extra, t_out_redirect_append_node *node)
{
}

void	visit_heredoc_in_node(
	t_visitor_extra *extra, t_heredoc_in_node *node)
{
}

void	visit_parenthesized_node(
	t_visitor_extra *extra, t_parenthesized_node *node)
{
}

void	visit_concat_node(
	t_visitor_extra *extra, t_concat_node *node)
{
}

void	visit_word_node(
	t_visitor_extra *extra, t_word_node *node)
{
}

void	visit_variable_node(
	t_visitor_extra *extra, t_variable_node *node)
{
	
}

void	visit_nodes(
	t_visitor_extra *extra, t_nodes *node)
{
	(void) extra;
	(void) node;
}

t_visitor	*interpreter_visitor(void)
{
	t_visitor	*visitor;

	visitor = malloc(sizeof(t_visitor));
	if (visitor == NULL)
		memory_error();
	visitor->visit_and_node = visit_and_node;
	visitor->visit_or_node = visit_or_node;
	visitor->visit_pipe_node = visit_pipe_node;
	visitor->visit_in_redirect_node = visit_in_redirect_node;
	visitor->visit_out_redirect_node = visit_out_redirect_node;
	visitor->visit_out_redirect_append_node = visit_out_redirect_append_node;
	visitor->visit_heredoc_in_node = visit_heredoc_in_node;
	visitor->visit_parenthesized_node = visit_parenthesized_node;
	visitor->visit_concat_node = visit_concat_node;
	visitor->visit_word_node = visit_word_node;
	visitor->visit_variable_node = visit_variable_node;
	visitor->visit_nope_node = visit_nope_node;
	visitor->visit_nodes = visit_nodes;
	return (visitor);
}
*/
// ---------------------------------------------

void	visualizer_and_node(
	t_visitor_extra *extra, t_and_node *node)
{
	printf("(");
	extra->visit(extra, node->left);
	printf(" && ");
	extra->visit(extra, node->right);
	printf(")");
}

void	visualizer_or_node(
	t_visitor_extra *extra, t_or_node *node)
{
	printf("(");
	extra->visit(extra, node->left);
	printf(" || ");
	extra->visit(extra, node->right);
	printf(")");
}

void	visualizer_pipe_node(
	t_visitor_extra *extra, t_pipe_node *node)
{
	printf("(");
	extra->visit(extra, node->left);
	printf(" | ");
	extra->visit(extra, node->right);
	printf(")");
}

void	visualizer_in_redirect_node(
	t_visitor_extra *extra, t_in_redirect_node *node)
{
	printf("(");
	printf("< ");
	extra->visit(extra, node->file_name);
	printf(" ");
	extra->visit(extra, node->operand);
	printf(")");
}

void	visualizer_out_redirect_node(
	t_visitor_extra *extra, t_out_redirect_node *node)
{
	printf("(");
	printf("> ");
	extra->visit(extra, node->file_name);
	printf(" ");
	extra->visit(extra, node->operand);
	printf(")");
}

void	visualizer_out_redirect_append_node(
	t_visitor_extra *extra, t_out_redirect_append_node *node)
{
	printf("(");
	printf(">> ");
	extra->visit(extra, node->file_name);
	printf(" ");
	extra->visit(extra, node->operand);
	printf(")");
}

void	visualizer_heredoc_in_node(
	t_visitor_extra *extra, t_heredoc_in_node *node)
{
	printf("(");
	printf("<< ");
	extra->visit(extra, node->limiter);
	printf(" ");
	extra->visit(extra, node->operand);
	printf(")");
}

void	visualizer_parenthesized_node(
	t_visitor_extra *extra, t_parenthesized_node *node)
{
	size_t	index;

	index = -1;
	printf("(");
	extra->visit(extra, node->nodes);
	printf(")");
}

void	visualizer_concat_node(
	t_visitor_extra *extra, t_concat_node *node)
{
	extra->visit(extra, node->left);
	extra->visit(extra, node->right);
}

void	visualizer_word_node(
	t_visitor_extra *extra, t_word_node *node)
{
	(void) extra;
	printf("%s", node->word);
}

void	visualizer_variable_node(
	t_visitor_extra *extra, t_variable_node *node)
{
	(void) extra;
	printf("$%s", node->name);
}

void	visualizer_nodes(
	t_visitor_extra *extra, t_nodes *node)
{
	size_t	index;

	if (node->size == 0)
		return ;
	printf("(");
	index = -1;
	while (++index < node->size)
	{
		if (index > 0)
			printf(" ");
		extra->visit(extra, node->nodes[index]);
	}
	printf(")");
}

t_visitor	*visualizer_visitor(void)
{
	t_visitor	*visitor;

	visitor = malloc(sizeof(t_visitor));
	if (visitor == NULL)
		memory_error();
	visitor->visit_and_node = visualizer_and_node;
	visitor->visit_or_node = visualizer_or_node;
	visitor->visit_pipe_node = visualizer_pipe_node;
	visitor->visit_in_redirect_node = visualizer_in_redirect_node;
	visitor->visit_out_redirect_node = visualizer_out_redirect_node;
	visitor->visit_out_redirect_append_node
		= visualizer_out_redirect_append_node;
	visitor->visit_heredoc_in_node = visualizer_heredoc_in_node;
	visitor->visit_parenthesized_node = visualizer_parenthesized_node;
	visitor->visit_concat_node = visualizer_concat_node;
	visitor->visit_word_node = visualizer_word_node;
	visitor->visit_variable_node = visualizer_variable_node;
	visitor->visit_nodes = visualizer_nodes;
	return (visitor);
}
