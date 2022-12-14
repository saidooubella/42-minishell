/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:26:53 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 21:47:14 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_CASES_H
# define LEXER_CASES_H

# include "lexer.h"

# define BONUS_SPECIAL_CHARS "\"'<|>()&$ \t\r\n\f\v"
# define SPECIAL_CHARS "\"'<|>$ \t\r\n\f\v"

size_t	tokenize_string(t_lexer *lexer, t_tokens *tokens, size_t start);

bool	lexer_expect(t_lexer *lexer, char expected, char *message);
bool	lexer_tokenize_token(t_lexer *lexer, t_tokens *tokens,
			t_token_type type, size_t length);
bool	lexer_tokenize_double_greater_than(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_double_ampersand(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_double_less_than(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_greater_than(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_parenthesize(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_double_pipe(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_dollar_sign(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_identifier(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_raw_string(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_less_than(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_string(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_pipe(t_lexer *lexer, t_tokens *tokens);
bool	lexer_tokenize_word(t_lexer *lexer, t_tokens *tokens);
bool	is_double_greater_than(t_lexer *lexer);
bool	is_double_ampersand(t_lexer *lexer);
bool	is_double_less_than(t_lexer *lexer);
bool	is_everything_else(t_lexer *lexer);
bool	is_parenthesize(t_lexer *lexer);
bool	is_double_quote(t_lexer *lexer);
bool	is_greater_than(t_lexer *lexer);
bool	is_single_quote(t_lexer *lexer);
bool	is_double_pipe(t_lexer *lexer);
bool	is_dollar_sign(t_lexer *lexer);
bool	is_identifier(t_lexer *lexer);
bool	is_less_than(t_lexer *lexer);
bool	is_pipe(t_lexer *lexer);

#endif
