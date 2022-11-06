/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:22:45 by soubella          #+#    #+#             */
/*   Updated: 2022/11/06 13:13:51 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENS_H
# define TOKENS_H

# include <stdbool.h>
# include <stddef.h>

typedef enum e_token_type
{
	DOUBLE_GREATER_THAN,
	EXIT_CODE_VARIABLE,
	DOUBLE_AMPERSAND,
	DOUBLE_LESS_THAN,
	CLOSE_PARENT,
	DOUBLE_QUOTE,
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

void		tokens_add(t_tokens *tokens, char *lexeme, t_token_type type);
void		tokens_free(t_tokens **tokens);
t_tokens	*tokens_new(void);

#endif