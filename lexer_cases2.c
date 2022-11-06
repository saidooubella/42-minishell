/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:29:07 by soubella          #+#    #+#             */
/*   Updated: 2022/11/04 10:59:12 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_cases.h"
#include "lexer.h"

void	lexer_tokenize_double_ampersand(t_lexer *lexer, t_tokens *tokens)
{
	lexer_tokenize_token(lexer, tokens, DOUBLE_AMPERSAND, 2);
}

void	lexer_tokenize_double_pipe(t_lexer *lexer, t_tokens *tokens)
{
	lexer_tokenize_token(lexer, tokens, DOUBLE_PIPE, 2);
}

void	lexer_tokenize_double_greater_than(t_lexer *lexer, t_tokens *tokens)
{
	lexer_tokenize_token(lexer, tokens, DOUBLE_GREATER_THAN, 2);
}

void	lexer_tokenize_double_less_than(t_lexer *lexer, t_tokens *tokens)
{
	lexer_tokenize_token(lexer, tokens, DOUBLE_LESS_THAN, 2);
}

void	lexer_tokenize_greater_than(t_lexer *lexer, t_tokens *tokens)
{
	lexer_tokenize_token(lexer, tokens, GREATER_THAN, 1);
}
