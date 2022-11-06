/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:29:37 by soubella          #+#    #+#             */
/*   Updated: 2022/11/04 10:59:19 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>

#include "lexer_cases.h"
#include "lexer.h"

void	lexer_tokenize_less_than(t_lexer *lexer, t_tokens *tokens)
{
	lexer_tokenize_token(lexer, tokens, LESS_THAN, 1);
}

void	lexer_tokenize_pipe(t_lexer *lexer, t_tokens *tokens)
{
	lexer_tokenize_token(lexer, tokens, PIPE, 1);
}

bool	is_double_ampersand(t_lexer *lexer)
{
	return (lexer_current(lexer) == '&' && lexer_ahead(lexer) == '&');
}

bool	is_double_pipe(t_lexer *lexer)
{
	return (lexer_current(lexer) == '|' && lexer_ahead(lexer) == '|');
}

bool	is_double_greater_than(t_lexer *lexer)
{
	return (lexer_current(lexer) == '>' && lexer_ahead(lexer) == '>');
}
