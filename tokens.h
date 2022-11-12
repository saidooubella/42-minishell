/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:22:45 by soubella          #+#    #+#             */
/*   Updated: 2022/11/12 18:54:33 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENS_H
# define TOKENS_H

# include <stdbool.h>
# include <stddef.h>

typedef enum e_token_type
{
	DOUBLE_GREATER_THAN,
	DOUBLE_AMPERSAND,
	DOUBLE_LESS_THAN,
	CLOSE_PARENT,
	OPEN_DOUBLE_QUOTE,
	CLOSE_DOUBLE_QUOTE,
	GREATER_THAN,
	DOUBLE_PIPE,
	END_OF_FILE,
	OPEN_PARENT,
	LESS_THAN,
	VARIABLE,
	DOLLAR,
	PIPE,
	PLUS,
	WORD,
}	t_token_type;

typedef struct s_token
{
	char			*lexeme;
	t_token_type	type;
}	t_token;

typedef struct s_tokens
{
	t_token	*tokens;
	size_t	capacity;
	size_t	size;
}	t_tokens;

typedef struct s_optional_token
{
	t_token	*token;
	bool	present;
}	t_optional_token;

t_optional_token	token_optional(t_token *token, bool present);
void				tokens_add(t_tokens *tokens, char *lexeme, t_token_type type);
void				tokens_free(t_tokens **tokens);
t_tokens			*tokens_new(void);

#endif