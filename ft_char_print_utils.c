/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_char_print_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 18:56:55 by soubella          #+#    #+#             */
/*   Updated: 2022/10/27 14:28:21 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "string_utils.h"

size_t	put_char(int fd, char character)
{
	write(fd, &character, 1);
	return (1);
}

size_t	put_string(int fd, char *string)
{
	size_t		string_size;

	if (string == 0)
		string = "(null)";
	string_size = string_length(string);
	write(fd, string, string_size);
	return (string_size);
}
