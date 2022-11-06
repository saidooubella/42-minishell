/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 18:57:09 by soubella          #+#    #+#             */
/*   Updated: 2022/10/27 19:39:22 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>

#include "ft_printf.h"

size_t	put_argument(int fd, char **format, va_list args)
{
	size_t			length;

	if (**format == 0)
		return (0);
	if (**format == 'd' || **format == 'i')
		length = put_i32(fd, va_arg(args, int), BASE_10);
	else if (**format == 'c')
		length = put_char(fd, (char) va_arg(args, int));
	else if (**format == 'z')
		length = put_u64(fd, va_arg(args, size_t), BASE_10, 10);
	else if (**format == 's')
		length = put_string(fd, va_arg(args, char *));
	else if (**format == 'p')
		length = put_address(fd, va_arg(args, void *));
	else if (**format == 'u')
		length = put_u32(fd, va_arg(args, unsigned int), BASE_10);
	else if (**format == 'x')
		length = put_u32(fd, va_arg(args, unsigned int), BASE_16_LOWER);
	else if (**format == 'X')
		length = put_u32(fd, va_arg(args, unsigned int), BASE_16_UPPER);
	else
		length = put_char(fd, **format);
	*format += 1;
	return (length);
}

int	ft_printf(int fd, const char *format, ...)
{
	int		result;
	va_list	args;

	va_start(args, format);
	result = ft_printf_raw(fd, format, args);
	va_end(args);
	return (result);
}

int	ft_printf_raw(int fd, const char *format, va_list args)
{
	int		size;

	size = 0;
	while (*format)
	{
		if (*format == '%')
		{
			format++;
			size += put_argument(fd, (char **) &format, args);
		}
		else
		{
			put_char(fd, *format++);
			size += 1;
		}
	}
	return (size);
}
