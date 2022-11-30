/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils_3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:32:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/30 20:30:19 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "string_utils.h"
#include "utils.h"

char	*string_duplicate(char *s1)
{
	char	*result;
	char	*temp;

	result = malloc(string_length(s1) + 1);
	if (result == NULL)
		memory_error();
	temp = result;
	while (*s1)
		*temp++ = *s1++;
	*temp = '\0';
	return (result);
}

char	*substring(char *string, size_t start, size_t end)
{
	char	*substr;
	size_t	length;
	size_t	index;

	length = end - start;
	substr = malloc(length + 1);
	if (substr == NULL)
		memory_error();
	index = 0;
	while (index < length)
	{
		substr[index] = string[start + index];
		index++;
	}
	substr[index] = '\0';
	return (substr);
}

bool	string_starts_with(char *str, char *to_find, size_t to_find_len)
{
	size_t	i;

	i = 0;
	while (str[i] && i < to_find_len)
	{
		if (str[i] != to_find[i])
			return (false);
		i++;
	}
	return (i == to_find_len);
}

ssize_t	string_index_of(char *target, char *str)
{
	size_t	str_len;
	size_t	index;

	index = 0;
	str_len = string_length(str);
	while (target[index])
	{
		if (string_starts_with(&target[index], str, str_len))
			return (index);
		index++;
	}
	return (-1);
}

char	*string_trim(char *str)
{
	size_t	start;
	size_t	end;

	start = 0;
	while (is_whitespace((unsigned char) str[start]))
		start++;
	if (str[start] == 0)
		return (string_duplicate(""));
	end = start + string_length(&str[start]) - 1;
	while (end > start && is_whitespace((unsigned char) str[end]))
		end--;
	return (substring(str, start, end + 1));
}
