/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_number_print_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 18:57:02 by soubella          #+#    #+#             */
/*   Updated: 2022/10/27 19:42:00 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>

#include "string_utils.h"
#include "ft_printf.h"

size_t	put_u64(int fd, unsigned long long number, char *base, size_t base_size)
{
	size_t	length;

	length = 0;
	if (number >= base_size)
		length = put_u64(fd, number / base_size, base, base_size);
	write(fd, &base[number % base_size], 1);
	return (length + 1);
}

size_t	put_i32_prefix(int fd, char *prefix, unsigned int number, char *base)
{
	size_t		prefix_size;

	prefix_size = string_length(prefix);
	write(fd, prefix, prefix_size);
	return (prefix_size + put_u64(fd, number, base, string_length(base)));
}

size_t	put_i32(int fd, int number, char *base)
{
	if (number < 0)
		return (put_i32_prefix(fd, "-", -number, base));
	return (put_i32_prefix(fd, "", number, base));
}

size_t	put_u32(int fd, unsigned int number, char *base)
{
	return (put_u64(fd, number, base, string_length(base)));
}

size_t	put_address(int fd, void *pointer)
{
	write(fd, "0x", 2);
	return (2 + put_u64(fd, (unsigned long long) pointer, BASE_16_LOWER, 16));
}
