/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:19:18 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 20:29:02 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <stdbool.h>
# include <stddef.h>

# include "environment.h"
# include "tokens.h"

# define CASES_SIZE 13

typedef struct s_lexer_case	t_lexer_case;

typedef struct s_lexer
{
	t_environment	*env;
	char			*content;
	t_lexer_case	*cases;
	size_t			length;
	size_t			index;
	size_t			whitespace_start;
	size_t			whitespace_end;
	bool			within_str;
}	t_lexer;

typedef bool				(*t_apply)(t_lexer *lexer, t_tokens *tokens);
typedef bool				(*t_check)(t_lexer *lexer);

typedef struct s_lexer_case
{
	t_apply	apply;
	t_check	check;
}	t_lexer_case;

typedef struct s_lexer_result
{
	t_tokens	*tokens;
	bool		success;
}	t_lexer_result;

t_lexer			*lexer_new(t_environment *env, char *content);
bool			lexer_reached_end(t_lexer *lexer);
t_lexer_result	lexer_tokenize(t_lexer *lexer);
char			lexer_current(t_lexer *lexer);
char			lexer_ahead(t_lexer *lexer);
void			lexer_free(t_lexer **lexer);

#endif
