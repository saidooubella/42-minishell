/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 11:33:26 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 11:20:51 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "string_builder.h"
#include "string_utils.h"
#include "utils.h"

char	*string_builder_to_cstr(t_string_builder *b)
{
	char	*string;
	size_t	index;

	string = malloc(b->size + 1);
	if (string == NULL)
		memory_error();
	index = -1;
	while (++index < b->size)
		string[index] = b->buffer[index];
	string[b->size] = '\0';
	return (string);
}

void	string_builder_append_cstring(
	t_string_builder *b, const char *str)
{
	string_builder_append_string(b, str, string_length(str));
}

void	string_builder_clear(t_string_builder *b)
{
	b->size = 0;
}

void	string_builder_free(t_string_builder **b)
{
	free((*b)->buffer);
	free(*b);
	*b = 0;
}
