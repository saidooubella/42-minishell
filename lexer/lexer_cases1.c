/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:28:08 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 16:24:18 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

#include "lexer_cases.h"
#include "string_utils.h"
#include "lexer_utils.h"
#include "lexer.h"
#include "utils.h"
#include "ft_printf.h"

bool	lexer_tokenize_token(t_lexer *lexer, t_tokens *tokens,
	t_token_type type, size_t length)
{
	size_t	start;

	start = lexer->index;
	lexer->index += length;
	tokens_smart_add(lexer, tokens, substring(lexer->content,
			start, lexer->index), type);
	return (true);
}

bool	lexer_tokenize_identifier(t_lexer *lexer, t_tokens *tokens)
{
	size_t	start;

	lexer_tokenize_token(lexer, tokens, DOLLAR, 1);
	start = lexer->index;
	if (lexer_current(lexer) == '?')
		lexer->index++;
	else
	{
		while (is_identifier_cont(lexer_current(lexer)))
			lexer->index++;
	}
	tokens_smart_add(lexer, tokens, substring(lexer->content,
			start, lexer->index), VARIABLE);
	return (true);
}

bool	lexer_tokenize_word(t_lexer *lexer, t_tokens *tokens)
{
	size_t	start;

	start = lexer->index;
	lexer->index += 1;
	while (!lexer_reached_end(lexer))
	{
		if (lexer->env->is_bonus
			&& string_contains(BONUS_SPECIAL_CHARS, lexer_current(lexer)))
			break ;
		if (!lexer->env->is_bonus
			&& string_contains(SPECIAL_CHARS, lexer_current(lexer)))
			break ;
		lexer->index += 1;
	}
	tokens_smart_add(lexer, tokens, substring(lexer->content,
			start, lexer->index), WORD);
	return (true);
}

bool	lexer_tokenize_raw_string(t_lexer *lexer, t_tokens *tokens)
{
	size_t	start;
	bool	found;

	lexer_tokenize_token(lexer, tokens, OPEN_SINGLE_QUOTE, 1);
	start = lexer->index;
	while (!lexer_reached_end(lexer) && lexer_current(lexer) != '\'')
		lexer->index += 1;
	tokens_smart_add(lexer, tokens, substring(lexer->content,
			start, lexer->index), WORD);
	found = lexer_expect(lexer, '\'', "unterminated string literal");
	if (found)
		lexer_tokenize_token(lexer, tokens, CLOSE_SINGLE_QUOTE, 1);
	return (found);
}

bool	lexer_tokenize_string(t_lexer *lexer, t_tokens *tokens)
{
	size_t	start;
	bool	found;

	lexer_tokenize_token(lexer, tokens, OPEN_DOUBLE_QUOTE, 1);
	start = lexer->index;
	while (!lexer_reached_end(lexer) && lexer_current(lexer) != '"')
	{
		if (is_identifier(lexer))
		{
			if (lexer->index - start > 0)
				tokens_smart_add(lexer, tokens, substring(lexer->content,
						start, lexer->index), WORD);
			lexer_tokenize_identifier(lexer, tokens);
			start = lexer->index;
		}
		else
			lexer->index++;
	}
	if (lexer->index - start > 0)
		tokens_smart_add(lexer, tokens, substring(lexer->content,
				start, lexer->index), WORD);
	found = lexer_expect(lexer, '"', "unterminated string literal");
	if (found)
		lexer_tokenize_token(lexer, tokens, CLOSE_DOUBLE_QUOTE, 1);
	return (found);
}
