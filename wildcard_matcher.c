/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_matcher.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:38:43 by soubella          #+#    #+#             */
/*   Updated: 2022/11/15 10:38:43 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdlib.h>

#include "string_utils.h"
#include "string_builder.h"
#include "string.h"

static size_t	index_until(char *str, int start, char *c)
{
	ssize_t index;

	index = string_index_of(str + start, c);
	if (index == -1)
		return (string_length(str));
	return (start + index);
}

static char	*substring_until(char *str, int start, char *c)
{
	return (substring(str, start, index_until(str, start, c)));
}

static bool matches(char *pattern, char *target)
{
	size_t	pattern_len;
	size_t	target_len;
	size_t	str_index;
	size_t	text_len;
	size_t	index;
	char	*substr;
	char	*temp;

	pattern_len = string_length(pattern);
	target_len = string_length(target);
	str_index = 0;
	index = 0;
	if (*pattern == '*' && *target == '.')
		return (false);
	while (index < pattern_len)
	{
		if (pattern[index] == '*')
		{
			if (index + 1 >= pattern_len || pattern[index + 1] != '*')
			{
				if (index == pattern_len - 1)
					return (true);
				temp = substring_until(pattern, index + 1, "*");
				str_index = index_until(target, str_index, temp);
				free(temp);
			}
			index++;
		}
		else
		{
			temp = substring_until(pattern, index, "*");
			text_len = string_length(temp);
			substr = substring(target, str_index, target_len);
			if (!string_starts_with(substr, temp, text_len))
			{
				free(temp);
				free(substr);
				return (false);
			}
			free(temp);
			free(substr);
			str_index += text_len;
			index += text_len;
		}
	}
	return (str_index == target_len);
}

t_string apply_pattern(char *pattern, char *directory)
{
	t_string_builder	*builder;
	struct dirent		*element;
	char				*result;
	DIR					*dir;
	size_t				index;

	if (string_index_of(pattern, "*") == -1)
		return (string_create(pattern, true));
	builder = string_builder_new();
	result	= pattern;
	index = -1;
	dir = opendir(directory);
	while (dir)
	{
		element = readdir(dir);
		if (element == NULL)
			break;
		if (matches(pattern, element->d_name))
		{
			if (builder->size > 0)
				string_builder_append_char(builder, ' ');
			string_builder_append_cstring(builder, element->d_name);
		}
	}
	if (dir != NULL)
		closedir(dir);
	if (builder->size > 0)
		result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	return (string_create(result, result != pattern));
}
