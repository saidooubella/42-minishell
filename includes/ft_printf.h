/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 18:57:15 by soubella          #+#    #+#             */
/*   Updated: 2022/10/30 11:04:58 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <sys/types.h>
# include <stdarg.h>
# include <unistd.h>

# define BASE_16_UPPER	"0123456789ABCDEF"
# define BASE_16_LOWER	"0123456789abcdef"
# define BASE_10		"0123456789"

size_t	put_i32(int fd, int number, char *base);
size_t	put_u64(int fd, unsigned long long number,
			char *base, size_t base_size);
size_t	put_i32_prefix(int fd, char *prefix, unsigned int number, char *base);
size_t	put_u32(int fd, unsigned int number, char *base);
size_t	put_string(int fd, char *string);
size_t	put_address(int fd, void *pointer);
size_t	put_char(int fd, char character);
int		ft_printf_raw(int fd, const char *format, va_list args);
int		ft_printf(int fd, const char *format, ...);

#endif
