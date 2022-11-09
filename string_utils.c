/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:41:23 by soubella          #+#    #+#             */
/*   Updated: 2022/11/09 13:44:50 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "string_utils.h"
#include "utils.h"

static char	*append(char *s1, char const *s2)
{
	while (*s2)
		*s1++ = *s2++;
	return (s1);
}

char	*string_join(char const *s1, char const *s2)
{
	size_t	size;
	char	*res;

	size = string_length(s1) + string_length(s2);
	res = malloc(size + 1);
	if (res == 0)
		error("Out of memory");
	*append(append(res, s1), s2) = 0;
	return (res);
}

int	to_lower(int ch)
{
	if ('A' <= ch && ch <= 'Z')
		return (ch + 32);
	return (ch);
}

int	string_compare(const char *s1, const char *s2, size_t n)
{
	unsigned char	*str1;
	unsigned char	*str2;
	size_t			i;

	if (n == 0)
		return (0);
	str1 = (unsigned char *) s1;
	str2 = (unsigned char *) s2;
	i = 0;
	while (i < n - 1 && str1[i] && str2[i] && str1[i] == str2[i])
		i++;
	return (str1[i] - str2[i]);
}

int	string_equals(char *s1, char *s2)
{
	size_t	s1_size;
	size_t	s2_size;

	s1_size = string_length(s1);
	s2_size = string_length(s2);
	if (s1_size == s2_size)
		if (string_compare(s1, s2, s1_size) == 0)
			return (1);
	return (0);
}

static size_t	calc_words_count(char *string, char sep)
{
	size_t	count;

	count = 0;
	while (1)
	{
		while (*string && *string == sep)
			string++;
		if (*string == 0)
			break ;
		count++;
		while (*string && *string != sep)
			string++;
	}
	return (count);
}

static char	*split_substring(
	char **strings, char **src_string, size_t *src_index, char sep)
{
	size_t	substring_length;
	char	*substring;
	char	*string;
	size_t	i;

	string = *src_string;
	substring_length = 0;
	while (string[substring_length] && string[substring_length] != sep)
		substring_length++;
	substring = malloc(sizeof(char) * (substring_length + 1));
	if (substring == 0)
	{
		while (*src_index >= 0)
			free(strings[(*src_index)--]);
		free(strings);
		return (0);
	}
	i = 0;
	while (*string && i < substring_length)
		substring[i++] = *string++;
	substring[i] = 0;
	*src_string = string;
	return (substring);
}

char	**string_split(char *string, char sep)
{
	char	*substring;
	size_t	words_count;
	char	**strings;
	size_t	index;

	words_count = calc_words_count(string, sep);
	strings = malloc(sizeof(char *) * (words_count + 1));
	if (strings == 0)
		memory_error();
	index = 0;
	while (index < words_count)
	{
		while (*string == sep)
			string++;
		substring = split_substring(strings, &string, &index, sep);
		if (substring == 0)
			memory_error();
		strings[index++] = substring;
	}
	strings[index] = 0;
	return (strings);
}

size_t	string_length(const char *string)
{
	size_t	length;

	if (string == NULL)
		return (0);
	length = -1;
	while (string[++length])
		;
	return (length);
}

bool	string_contains(char const *s1, char ch)
{
	while (*s1)
		if (*s1++ == ch)
			return (true);
	return (false);
}

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
	*temp = 0;
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
