/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 13:43:50 by soubella          #+#    #+#             */
/*   Updated: 2022/11/08 13:24:23 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>

#include "string_utils.h"
#include "ft_printf.h"
#include "lexer.h"

static bool	is_other_special(t_token_type type)
{
	if (type == DOUBLE_GREATER_THAN || type == DOUBLE_AMPERSAND
		|| type == DOUBLE_LESS_THAN || type == OPEN_PARENT
		|| type == CLOSE_PARENT || type == GREATER_THAN
		|| type == DOUBLE_PIPE || type == END_OF_FILE
		|| type == CLOSE_DOUBLE_QUOTE)
		return (true);
	return (false);
}

static bool	is_self_special(t_token_type type)
{
	if (type == DOUBLE_GREATER_THAN || type == DOUBLE_AMPERSAND
		|| type == DOUBLE_LESS_THAN || type == OPEN_PARENT
		|| type == CLOSE_PARENT || type == GREATER_THAN
		|| type == DOUBLE_PIPE || type == END_OF_FILE
		|| type == OPEN_DOUBLE_QUOTE || type == DOLLAR)
		return (true);
	return (false);
}

void	tokens_smart_add(
	t_lexer *lexer, t_tokens *tokens, char *lexeme, t_token_type type)
{
	if (lexer->whitespace_start != 0
		&& lexer->whitespace_end - lexer->whitespace_start == 0
		&& !is_self_special(tokens->tokens[tokens->size - 1].type)
		&& !is_other_special(type))
		tokens_add(tokens, string_duplicate("+"), PLUS);
	tokens_add(tokens, lexeme, type);
}

void	error(char *fmt, ...)
{
	va_list	args;

	write(STDERR_FILENO, "Error: ", 7);
	va_start(args, fmt);
	ft_printf_raw(STDERR_FILENO, fmt, args);
	va_end(args);
	write(STDERR_FILENO, "\n", 1);
	exit(1);
}

void	memory_error(void)
{
	write(STDOUT_FILENO, "Error: Program run out of memory\n", 33);
	exit(1);
}
