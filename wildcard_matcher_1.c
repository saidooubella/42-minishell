/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_matcher_1.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:38:43 by soubella          #+#    #+#             */
/*   Updated: 2022/11/26 15:19:53 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdlib.h>

#include "string_utils.h"
#include "string_builder.h"
#include "wildcard_matcher.h"
#include "string.h"

static bool	matches(char *pattern, char *target)
{
	t_matcher_locals	locals;

	locals.pattern_len = string_length(pattern);
	locals.target_len = string_length(target);
	locals.str_index = 0;
	locals.index = 0;
	if (*pattern == '*' && *target == '.')
		return (false);
	while (locals.index < locals.pattern_len)
	{
		if (pattern[locals.index] == '*')
		{
			if (wildcard_case(pattern, target, &locals))
				return (true);
		}
		else
		{
			if (!text_case(pattern, target, &locals))
				return (false);
		}
	}
	return (locals.str_index == locals.target_len);
}

static char	*apply_pattern_epilogue(
	char *pattern, DIR *dir, t_string_builder *builder)
{
	char	*result;

	result = pattern;
	if (dir != NULL)
		closedir(dir);
	if (builder->size > 0)
		result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	return (result);
}

t_string	apply_pattern(char *pattern, char *directory)
{
	t_string_builder	*builder;
	struct dirent		*element;
	char				*result;
	DIR					*dir;

	if (string_index_of(pattern, "*") == -1)
		return (string_create(pattern, true));
	builder = string_builder_new();
	dir = opendir(directory);
	while (dir)
	{
		element = readdir(dir);
		if (element == NULL)
			break ;
		if (matches(pattern, element->d_name))
		{
			if (builder->size > 0)
				string_builder_append_char(builder, ' ');
			string_builder_append_cstring(builder, element->d_name);
		}
	}
	result = apply_pattern_epilogue(pattern, dir, builder);
	return (string_create(result, true));
}
