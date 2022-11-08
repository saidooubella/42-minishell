/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:28:08 by soubella          #+#    #+#             */
/*   Updated: 2022/11/08 15:16:20 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "lexer_cases.h"
#include "string_utils.h"
#include "lexer_utils.h"
#include "lexer.h"
#include "utils.h"

void	lexer_tokenize_token(t_lexer *lexer, t_tokens *tokens,
	t_token_type type, size_t length)
{
	size_t	start;

	start = lexer->index;
	lexer->index += length;
	tokens_smart_add(lexer, tokens, substring(lexer->content,
			start, lexer->index), type);
}

void	lexer_tokenize_identifier(t_lexer *lexer, t_tokens *tokens)
{
	size_t	start;

	lexer_tokenize_token(lexer, tokens, DOLLAR, 1);
	start = lexer->index;
	if (lexer_current(lexer) == '?')
	{
		lexer->index++;
		tokens_smart_add(lexer, tokens, substring(lexer->content,
				start, lexer->index), EXIT_CODE_VARIABLE);
	}
	else
	{
		while (is_identifier_cont(lexer_current(lexer)))
			lexer->index++;
		tokens_smart_add(lexer, tokens, substring(lexer->content,
				start, lexer->index), VARIABLE);
	}
}

void	lexer_tokenize_word(t_lexer *lexer, t_tokens *tokens)
{
	size_t	start;

	start = lexer->index;
	while (!lexer_reached_end(lexer) && !string_contains(SPECIAL_CHARS,
			lexer_current(lexer)))
		lexer->index += 1;
	tokens_smart_add(lexer, tokens, substring(lexer->content,
			start, lexer->index), WORD);
}

void	lexer_tokenize_raw_string(t_lexer *lexer, t_tokens *tokens)
{
	size_t	start;

	lexer->index++;
	start = lexer->index;
	while (!lexer_reached_end(lexer) && lexer_current(lexer) != '\'')
		lexer->index += 1;
	tokens_smart_add(lexer, tokens, substring(lexer->content,
			start, lexer->index), WORD);
	if (lexer_current(lexer) == '\'')
		lexer->index++;
	else
		error("Error: unterminated string literal\n");
}

void	lexer_tokenize_string(t_lexer *lexer, t_tokens *tokens)
{
	size_t	start;

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
	if (lexer_current(lexer) == '"')
		lexer_tokenize_token(lexer, tokens, CLOSE_DOUBLE_QUOTE, 1);
	else
		error("Error: unterminated string literal\n");
}
