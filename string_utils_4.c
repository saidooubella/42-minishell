/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils_4.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:34:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:37:43 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>

#include "string_utils.h"
#include "utils.h"

int	are_equals(char *s1, char *s2)
{
	size_t	s1_size;
	size_t	s2_size;

	s1_size = string_length(s1);
	s2_size = string_length(s2);
	if (s1_size == s2_size || (s1_size == s2_size - 1
			&& s2[s2_size - 1] == '\n'))
		if (!string_compare(s1, s2, s1_size))
			return (1);
	return (0);
}

size_t	calc_words_count(char *string, char *seps)
{
	size_t	count;

	count = 0;
	while (1)
	{
		while (*string && string_contains(seps, *string))
			string++;
		if (*string == 0)
			break ;
		count++;
		while (*string && !string_contains(seps, *string))
			string++;
	}
	return (count);
}

static char	*split_substring(
	char **strings, char **src_string, size_t *src_index, char *seps)
{
	size_t	substring_length;
	char	*substring;
	char	*string;
	size_t	i;

	string = *src_string;
	substring_length = 0;
	while (string[substring_length]
		&& !string_contains(seps, string[substring_length]))
		substring_length++;
	substring = malloc(sizeof(char) * (substring_length + 1));
	if (substring == NULL)
	{
		(*src_index)--;
		while (*src_index >= 0)
			free(strings[(*src_index)--]);
		free(strings);
		return (NULL);
	}
	i = 0;
	while (*string && i < substring_length)
		substring[i++] = *string++;
	substring[i] = '\0';
	*src_string = string;
	return (substring);
}

char	**string_split(char *string, char *seps)
{
	char	*substring;
	size_t	words_count;
	char	**strings;
	size_t	index;

	words_count = calc_words_count(string, seps);
	strings = malloc(sizeof(char *) * (words_count + 1));
	if (strings == NULL)
		memory_error();
	index = 0;
	while (index < words_count)
	{
		while (*string && string_contains(seps, *string))
			string++;
		substring = split_substring(strings, &string, &index, seps);
		if (substring == NULL)
			memory_error();
		strings[index++] = substring;
	}
	strings[index] = NULL;
	return (strings);
}
