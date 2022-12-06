/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_cases4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:34:42 by soubella          #+#    #+#             */
/*   Updated: 2022/11/04 10:34:56 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>

#include "lexer.h"

bool	is_double_less_than(t_lexer *lexer)
{
	return (lexer_current(lexer) == '<' && lexer_ahead(lexer) == '<');
}

bool	is_greater_than(t_lexer *lexer)
{
	return (lexer_current(lexer) == '>');
}

bool	is_less_than(t_lexer *lexer)
{
	return (lexer_current(lexer) == '<');
}

bool	is_pipe(t_lexer *lexer)
{
	return (lexer_current(lexer) == '|');
}

bool	is_double_quote(t_lexer *lexer)
{
	return (lexer_current(lexer) == '"');
}
