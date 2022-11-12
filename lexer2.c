/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:47:53 by soubella          #+#    #+#             */
/*   Updated: 2022/11/12 20:08:28 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "string_utils.h"
#include "lexer_utils.h"
#include "tokens.h"
#include "lexer.h"
#include "utils.h"

bool	lexer_reached_end(t_lexer *lexer)
{
	return (lexer->index >= lexer->length);
}

static bool	skip_whitespace_check_end(t_lexer *lexer)
{
	lexer->whitespace_start = lexer->index;
	while (is_space(lexer_current(lexer)))
		lexer->index++;
	lexer->whitespace_end = lexer->index;
	return (!lexer_reached_end(lexer));
}

t_lexer_result	lexer_tokenize(t_lexer *lexer)
{
	size_t			index;
	t_lexer_result	result;

	result.tokens = tokens_new();
	result.success = true;
	while (result.success && skip_whitespace_check_end(lexer))
	{
		index = -1;
		while (++index < CASES_SIZE)
		{
			if (lexer->cases[index].check(lexer))
			{
				if (!lexer->cases[index].apply(lexer, result.tokens))
					result.success = false;
				break ;
			}
		}
	}
	tokens_smart_add(lexer, result.tokens,
		string_duplicate("end of file"), END_OF_FILE);
	return (result);
}

void	lexer_free(t_lexer **lexer)
{
	free((*lexer)->content);
	free((*lexer)->cases);
	free((*lexer));
	*lexer = NULL;
}
