/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:35:47 by soubella          #+#    #+#             */
/*   Updated: 2022/11/09 12:52:56 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>

#include "lexer_utils.h"
#include "lexer_cases.h"
#include "utils.h"
#include "lexer.h"

void	lexer_tokenize_parenthesize(t_lexer *lexer, t_tokens *tokens)
{
	if (lexer_current(lexer) == '(')
		lexer_tokenize_token(lexer, tokens, OPEN_PARENT, 1);
	else if (lexer_current(lexer) == ')')
		lexer_tokenize_token(lexer, tokens, CLOSE_PARENT, 1);
	else
		error("Error: Illegal state in 'lexer_tokenize_parenthesize'");
}

bool	is_parenthesize(t_lexer *lexer)
{
	return (lexer_current(lexer) == '(' || lexer_current(lexer) == ')');
}

bool	is_single_quote(t_lexer *lexer)
{
	return (lexer_current(lexer) == '\'');
}

bool	is_identifier(t_lexer *lexer)
{
	return (lexer_current(lexer) == '$' && (lexer_ahead(lexer) == '?'
			|| is_identifier_start(lexer_ahead(lexer))));
}

bool	is_everything_else(t_lexer *lexer)
{
	(void) lexer;
	return (true);
}
