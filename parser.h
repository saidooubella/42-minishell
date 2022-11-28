/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 10:58:25 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:23:41 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "string_builder.h"
# include "interpreter.h"
# include "tokens.h"
# include "string.h"
# include "nodes.h"

typedef struct s_parser
{
	t_environment	*env;
	t_tokens		*tokens;
	size_t			index;
}	t_parser;

t_optional_node		conjuction_expression(t_parser *parser);

bool				redirect_in_expr(t_parser *parser, t_node *command);
bool				is_unit_expr_beg(t_parser *parser);
t_optional_node		validate_command(t_parser *parser, t_node *command);
int					command_expr_unit(t_parser *parser, t_node *command);
t_optional_node		command_expr(t_parser *parser);

t_optional_elements	read_from_stdin(char *limiter, bool expand_vars);
t_optional_node		parent_expr(t_parser *parser);
bool				redirect_out_append_expr(t_parser *parser, t_node *command);
bool				heredoc_expr(t_parser *parser, t_node *command);
bool				redirect_out_expr(t_parser *parser, t_node *command);

t_optional_elements	unit_expression(
						t_parser *parser, bool var_expantion,
						bool *wildcard_expantion, bool *has_quotes);
t_elements			*concatenation_expression(
						t_parser *parser, bool expand, bool *has_quotes);
t_optional_elements	redirection_operand(
						t_parser *parser, bool expand, bool *has_quotes);
char				*create_var(
						t_elements *elements,
						char *line, t_string_builder *builder);

t_parser			*parser_new(t_tokens *tokens, t_environment *env);
void				parser_free(t_parser **parser);
bool				parser_reached_end(t_parser *parser);
bool				parser_current_is(t_parser *parser, t_token_type type);
t_token				*parser_consume(t_parser *parser);

t_optional_token	parser_expect(
						t_parser *parser, t_token_type type, char *expected);
t_string			current_lexeme(t_parser *parser);
t_elements			*word_expr(t_parser *parser);
t_elements			*var_expr(t_parser *parser, bool var_expantion);
t_elements			*string_expr(t_parser *parser, bool var_expantion);

t_optional_node		parse(t_parser *parser);
t_parser			*parser_new(t_tokens *tokens, t_environment *env);
void				parser_free(t_parser **parser);

#endif
