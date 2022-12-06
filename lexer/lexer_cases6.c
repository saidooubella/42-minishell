/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases6.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/05 17:15:01 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 17:56:21 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "lexer_cases.h"
#include "ft_printf.h"
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

bool	lexer_expect(t_lexer *lexer, char expected, char *message)
{
	if (lexer_current(lexer) == expected)
		return (true);
	ft_printf(STDERR_FILENO, "Error: %s\n", message);
	return (false);
}
