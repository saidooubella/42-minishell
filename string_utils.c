/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:41:23 by soubella          #+#    #+#             */
/*   Updated: 2022/11/15 18:47:02 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "string_utils.h"
#include "utils.h"

static int	digits_count(unsigned int number)
{
	int	count;

	if (number == 0)
		return (1);
	count = 0;
	while (number != 0)
	{
		number /= 10;
		count++;
	}
	return (count);
}

char	*llong_to_string(long long n)
{
	unsigned long long	number;
	char				*result;
	int					size;
	int					end;
	int					i;

	number = n;
	end = 0;
	if (n < 0)
	{
		number = -n;
		end = 1;
	}
	size = digits_count(number) + 1;
	if (n < 0)
		size += 1;
	result = malloc(sizeof(char) * size);
	if (result == 0)
		error("Out of memory");
	if (n < 0)
		result[0] = '-';
	i = size - 2;
	while (i >= end)
	{
		result[i--] = "0123456789"[number % 10];
		number /= 10;
	}
	result[size - 1] = 0;
	return (result);
}

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

static int	contains(char *sequence, char target)
{
	while (*sequence)
		if (*sequence++ == target)
			return (1);
	return (0);
}

static size_t	calc_words_count(char *string, char *seps)
{
	size_t	count;

	count = 0;
	while (1)
	{
		while (*string && contains(seps, *string))
			string++;
		if (*string == 0)
			break ;
		count++;
		while (*string && !contains(seps, *string))
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
	while (string[substring_length] && !contains(seps, string[substring_length]))
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
		while (*string && contains(seps, *string))
			string++;
		substring = split_substring(strings, &string, &index, seps);
		if (substring == NULL)
			memory_error();
		strings[index++] = substring;
	}
	strings[index] = NULL;
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

bool	is_whitespace(char c)
{
	return (c == '\t' || c == '\n' || c == '\r'
		|| c == ' ' || c == '\v' || c == '\f');
}

bool	is_digit(char c)
{
	return ('0' <= c && c <= '9');
}

long long	string_to_llong(char *str, bool *error)
{
	int					sign;
	unsigned long long	temp;
	unsigned long long	res;

	*error = false;
	sign = 1;
	res = 0;
	while (is_whitespace(*str))
		str++;
	if (*str == '+' || *str == '-')
		if (*str++ == '-')
			sign *= -1;
	if (*str == '\0')
		*error = true;
	while (is_digit(*str))
	{
		temp = (res * 10) + (*str++ - '0');
		if (temp < res || (sign == 1 && temp > _LONG_MAX) || (sign == -1 && temp > _LONG_MIN))
		{
			*error = true;
			return (res * sign);
		}
		res = temp;
	}
	if (*str != '\0')
		*error = true;
	return (res * sign);
}

void	*bytes_copy(void *dst, const void *src, size_t n)
{
	size_t	index;
	char	*from;
	char	*to;

	from = (char *) src;
	to = (char *) dst;
	index = -1;
	if (dst != src && n != 0)
		while (++index < n)
			to[index] = from[index];
	return (dst);
}

void	*bytes_move(void *dst, const void *src, size_t len)
{
	char	*from;
	char	*to;

	from = (char *) src;
	to = (char *) dst;
	if (dst == src || len == 0)
		return (dst);
	if (to > from)
	{
		from += len - 1;
		to += len - 1;
		while (from >= (char *) src)
			*to-- = *from--;
	}
	else
		bytes_copy(dst, src, len);
	return (dst);
}
