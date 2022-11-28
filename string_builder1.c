/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 11:26:52 by soubella          #+#    #+#             */
/*   Updated: 2022/11/24 21:44:48 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "string_builder.h"
#include "utils.h"

t_string_builder	*string_builder_new_cap(size_t capacity)
{
	t_string_builder	*builder;

	builder = malloc(sizeof(t_string_builder));
	if (builder == NULL)
		memory_error();
	builder->buffer = malloc(capacity);
	if (builder->buffer == NULL)
		memory_error();
	builder->capacity = capacity;
	builder->size = 0;
	return (builder);
}

t_string_builder	*string_builder_new(void)
{
	return (string_builder_new_cap(DEFAULT_CAPACITY));
}

static void	string_builder_ensure_capacity(t_string_builder *b, size_t size)
{
	char	*new_buffer;
	size_t	new_capacity;
	size_t	index;

	if (b->capacity > b->size + size)
		return ;
	new_capacity = (b->capacity << 1) + size;
	new_buffer = malloc(new_capacity);
	if (new_buffer == NULL)
		memory_error();
	index = -1;
	while (++index < b->size)
		new_buffer[index] = b->buffer[index];
	free(b->buffer);
	b->capacity = new_capacity;
	b->buffer = new_buffer;
}

void	string_builder_append_string(
	t_string_builder *b, const char *str, size_t size)
{
	size_t	index;

	string_builder_ensure_capacity(b, size);
	index = 0;
	while (index < size)
		b->buffer[b->size++] = str[index++];
}

void	string_builder_append_char(t_string_builder *b, char chr)
{
	string_builder_ensure_capacity(b, 1);
	b->buffer[b->size++] = chr;
}
