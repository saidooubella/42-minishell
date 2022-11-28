/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_matcher_2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:27:31 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:29:48 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>

#include "wildcard_matcher.h"
#include "string_utils.h"

static size_t	index_until(char *str, int start, char *c)
{
	ssize_t	index;

	index = string_index_of(str + start, c);
	if (index == -1)
		return (string_length(str));
	return (start + index);
}

static char	*substring_until(char *str, int start, char *c)
{
	return (substring(str, start, index_until(str, start, c)));
}

bool	wildcard_case(char *pattern, char *target, t_matcher_locals *locals)
{
	if (locals->index + 1 >= locals->pattern_len
		|| pattern[locals->index + 1] != '*')
	{
		if (locals->index == locals->pattern_len - 1)
			return (true);
		locals->temp = substring_until(pattern, locals->index + 1, "*");
		locals->str_index = index_until(
				target, locals->str_index, locals->temp);
		free(locals->temp);
	}
	locals->index++;
	return (false);
}

bool	text_case(char *pattern, char *target, t_matcher_locals *locals)
{
	locals->temp = substring_until(pattern, locals->index, "*");
	locals->text_len = string_length(locals->temp);
	locals->substr = substring(target, locals->str_index, locals->target_len);
	if (!string_starts_with(locals->substr, locals->temp, locals->text_len))
	{
		free(locals->temp);
		free(locals->substr);
		return (false);
	}
	free(locals->temp);
	free(locals->substr);
	locals->str_index += locals->text_len;
	locals->index += locals->text_len;
	return (true);
}
