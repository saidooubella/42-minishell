/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:19:18 by soubella          #+#    #+#             */
/*   Updated: 2022/11/06 12:28:47 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <stdbool.h>
# include <stddef.h>

# include "tokens.h"

# define CASES_SIZE 13

typedef struct s_lexer_case	t_lexer_case;

typedef struct s_lexer
{
	char			*content;
	t_lexer_case	*cases;
	size_t			length;
	size_t			index;
	size_t			whitespace_start;
	size_t			whitespace_end;
}	t_lexer;

typedef struct s_lexer_case
{
	void	(*apply)(t_lexer *lexer, t_tokens *tokens);
	bool	(*check)(t_lexer * lexer);
}	t_lexer_case;

bool		lexer_reached_end(t_lexer *lexer);
t_tokens	*lexer_tokenize(t_lexer *lexer);
char		lexer_current(t_lexer *lexer);
char		lexer_ahead(t_lexer *lexer);
void		lexer_free(t_lexer **lexer);
t_lexer		*lexer_new(char *content);

#endif
