/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils_1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:41:23 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:35:40 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "string_utils.h"
#include "utils.h"

static size_t	digits_count(long long number)
{
	size_t	count;

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

char	*build_string(uint64_t number, bool is_neg, size_t size)
{
	char	*result;
	ssize_t	index;
	ssize_t	end;

	result = malloc(sizeof(char) * size);
	if (result == 0)
		error("Out of memory");
	end = 0;
	if (is_neg)
	{
		result[0] = '-';
		end = 1;
	}
	index = size - 2;
	while (index >= end)
	{
		result[index--] = "0123456789"[number % 10];
		number /= 10;
	}
	result[size - 1] = 0;
	return (result);
}

char	*llong_to_string(long long n)
{
	uint64_t	number;
	size_t		size;

	number = n;
	if (n < 0)
		number = -n;
	size = digits_count(number) + 1;
	if (n < 0)
		size += 1;
	return (build_string(number, n < 0, size));
}

bool	string_contains(char const *s1, char ch)
{
	while (*s1)
		if (*s1++ == ch)
			return (true);
	return (false);
}
