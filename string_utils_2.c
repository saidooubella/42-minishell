/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:30:29 by soubella          #+#    #+#             */
/*   Updated: 2022/11/30 19:03:54 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdint.h>

#include "string_utils.h"

bool	is_whitespace(char c)
{
	return (c == '\t' || c == '\n' || c == '\r'
		|| c == ' ' || c == '\v' || c == '\f');
}

static bool	check_overflow(
	int32_t sign, uint64_t res, uint64_t temp, bool *error)
{
	if (temp < res || (sign == 1 && temp > _LONG_MAX)
		|| (sign == -1 && temp > _LONG_MIN))
		*error = true;
	return (*error);
}

int64_t	string_to_llong(char *str, bool allow_neg, bool *error)
{
	int32_t		sign;
	uint64_t	temp;
	uint64_t	res;

	*error = false;
	sign = 1;
	res = 0;
	while (is_whitespace(*str))
		str++;
	if (allow_neg && (*str == '+' || *str == '-'))
		if (*str++ == '-')
			sign *= -1;
	if (*str == '\0')
		*error = true;
	while ('0' <= *str && *str <= '9')
	{
		temp = (res * 10) + (*str++ - '0');
		if (check_overflow(sign, res, temp, error))
			return (-1);
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
