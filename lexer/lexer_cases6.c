/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases6.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/05 17:15:01 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 21:48:41 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "string_utils.h"
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

size_t	tokenize_string(t_lexer *lexer, t_tokens *tokens, size_t start)
{
	while (!lexer_reached_end(lexer) && lexer_current(lexer) != '"')
	{
		if (is_identifier(lexer))
		{
			if (lexer->index - start > 0)
				tokens_add(tokens, substring(lexer->content,
						start, lexer->index), WORD);
			lexer_tokenize_identifier(lexer, tokens);
			start = lexer->index;
		}
		else
			lexer->index++;
	}
	return (start);
}
