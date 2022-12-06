/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:35:47 by soubella          #+#    #+#             */
/*   Updated: 2022/12/03 14:29:47 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdbool.h>

#include "lexer_utils.h"
#include "lexer_cases.h"
#include "utils.h"
#include "lexer.h"
#include "ft_printf.h"

bool	lexer_tokenize_parenthesize(t_lexer *lexer, t_tokens *tokens)
{
	if (lexer_current(lexer) == '(')
	{
		lexer_tokenize_token(lexer, tokens, OPEN_PARENT, 1);
		return (true);
	}
	if (lexer_current(lexer) == ')')
	{
		lexer_tokenize_token(lexer, tokens, CLOSE_PARENT, 1);
		return (true);
	}
	error("Error: Illegal state in 'lexer_tokenize_parenthesize'");
	return (false);
}

bool	is_parenthesize(t_lexer *lexer)
{
	if (lexer->env->is_bonus)
		return (lexer_current(lexer) == '(' || lexer_current(lexer) == ')');
	return (false);
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
