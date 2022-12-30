/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:08:51 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 20:29:14 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lexer_cases.h"
#include "string_utils.h"
#include "lexer.h"
#include "utils.h"

void	set_case(t_lexer_case *caze, t_check check, t_apply apply)
{
	caze->apply = apply;
	caze->check = check;
}

t_lexer_case	*init_lexer_cases(void)
{
	t_lexer_case	*cases;
	size_t			index;

	cases = malloc(sizeof(t_lexer_case) * CASES_SIZE);
	if (cases == NULL)
		memory_error();
	index = -1;
	set_case(&cases[++index], is_double_greater_than,
		lexer_tokenize_double_greater_than);
	set_case(&cases[++index], is_double_less_than,
		lexer_tokenize_double_less_than);
	set_case(&cases[++index], is_double_ampersand,
		lexer_tokenize_double_ampersand);
	set_case(&cases[++index], is_double_pipe, lexer_tokenize_double_pipe);
	set_case(&cases[++index], is_greater_than, lexer_tokenize_greater_than);
	set_case(&cases[++index], is_parenthesize, lexer_tokenize_parenthesize);
	set_case(&cases[++index], is_single_quote, lexer_tokenize_raw_string);
	set_case(&cases[++index], is_double_quote, lexer_tokenize_string);
	set_case(&cases[++index], is_less_than, lexer_tokenize_less_than);
	set_case(&cases[++index], is_pipe, lexer_tokenize_pipe);
	set_case(&cases[++index], is_identifier, lexer_tokenize_identifier);
	set_case(&cases[++index], is_dollar_sign, lexer_tokenize_dollar_sign);
	set_case(&cases[++index], is_everything_else, lexer_tokenize_word);
	return (cases);
}

t_lexer	*lexer_new(t_environment *env, char *content)
{
	t_lexer	*lexer;

	lexer = malloc(sizeof(t_lexer));
	if (lexer == NULL)
		memory_error();
	lexer->length = string_length(content);
	lexer->cases = init_lexer_cases();
	lexer->within_str = false;
	lexer->content = content;
	lexer->index = 0;
	lexer->env = env;
	return (lexer);
}

char	lexer_current(t_lexer *lexer)
{
	if (!lexer_reached_end(lexer))
		return (lexer->content[lexer->index]);
	return ('\0');
}

char	lexer_ahead(t_lexer *lexer)
{
	if (lexer->index + 1 < lexer->length)
		return (lexer->content[lexer->index + 1]);
	return ('\0');
}
