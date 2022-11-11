/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:22:24 by soubella          #+#    #+#             */
/*   Updated: 2022/11/11 10:13:47 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "utils.h"

t_tokens	*tokens_new(void)
{
	t_tokens	*tokens;

	tokens = malloc(sizeof(t_tokens));
	if (tokens == NULL)
		memory_error();
	tokens->tokens = malloc(sizeof(t_token) * 16);
	if (tokens->tokens == NULL)
		memory_error();
	tokens->capacity = 16;
	tokens->size = 0;
	return (tokens);
}

t_optional_token	token_optional(t_token *token, bool present)
{
	t_optional_token	optional;

	optional.present = present;
	optional.token = token;
	return (optional);
}

void	tokens_free(t_tokens **tokens)
{
	size_t	index;

	index = -1;
	while (++index < (*tokens)->size)
		free((*tokens)->tokens[index].lexeme);
	free((*tokens)->tokens);
	free((*tokens));
	*tokens = NULL;
}

void	tokens_add(t_tokens *tokens, char *lexeme, t_token_type type)
{
	size_t	new_capacity;
	t_token	*new_tokens;

	if (tokens->size + 1 >= tokens->capacity)
	{
		new_capacity = (tokens->size + 1) << 1;
		new_tokens = malloc(sizeof(t_token) * new_capacity);
		if (new_tokens == NULL)
			memory_error();
		memcpy(new_tokens, tokens->tokens, tokens->size * sizeof(t_token));
		free(tokens->tokens);
		tokens->capacity = new_capacity;
		tokens->tokens = new_tokens;
	}
	tokens->tokens[tokens->size].lexeme = lexeme;
	tokens->tokens[tokens->size].type = type;
	tokens->size++;
}
