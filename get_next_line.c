/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/07 14:47:40 by soubella          #+#    #+#             */
/*   Updated: 2022/11/10 14:50:13 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "string_builder.h"
#include "get_next_line.h"

static t_read_info	line_info(char *string, size_t offset, size_t size)
{
	t_read_info	info;
	size_t		index;

	info.has_newline = 0;
	info.line_size = 0;
	index = offset;
	while (index < size)
	{
		info.line_size += 1;
		if (string[index] == '\n')
		{
			info.has_newline = 1;
			return (info);
		}
		index += 1;
	}
	return (info);
}

static int	read_line(int fd, t_string_builder *builder, t_state_holder *state)
{
	t_read_info	info;

	while (1)
	{
		if (state->offset >= (size_t) state->read_size)
		{
			state->read_size = read(fd, state->buffer, BUFFER_SIZE);
			state->offset = 0;
			if (state->read_size == 0 || state->read_size == -1)
			{
				state->read_size = 0;
				if (state->read_size == -1)
					return (0);
				return (1);
			}
		}
		info = line_info(state->buffer, state->offset, state->read_size);
		if (!string_builder_append_string(builder, state->buffer + state->offset
				, info.line_size))
			return (0);
		state->offset += info.line_size;
		if (info.has_newline)
			return (1);
	}
}

char	*get_next_line(int fd)
{
	static t_state_holder	state;
	t_string_builder		*builder;
	char					*result;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (0);
	builder = string_builder_new();
	if (builder == 0)
		return (0);
	result = 0;
	if (read_line(fd, builder, &state) && builder->size > 0)
		result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	return (result);
}
