/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils_5.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:35:15 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:38:11 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>

#include "string_utils.h"
#include "utils.h"

static char	*append(char *s1, char const *s2)
{
	if (s2 == NULL)
		return (s1);
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
		memory_error();
	*append(append(res, s1), s2) = 0;
	return (res);
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
