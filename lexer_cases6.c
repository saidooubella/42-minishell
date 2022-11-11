/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases6.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/05 17:15:01 by soubella          #+#    #+#             */
/*   Updated: 2022/11/11 11:26:19 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_cases.h"
#include "lexer.h"

bool	lexer_tokenize_dollar_sign(t_lexer *lexer, t_tokens *tokens)
{
	lexer_tokenize_token(lexer, tokens, WORD, 1);
	return (true);
}

bool	is_dollar_sign(t_lexer *lexer)
{
	return (lexer_current(lexer) == '$');
}
