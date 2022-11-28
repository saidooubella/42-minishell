/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_6.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 11:17:22 by soubella          #+#    #+#             */
/*   Updated: 2022/11/27 11:31:34 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "string_utils.h"
#include "interpreter.h"

t_to_be_closed	*to_be_closed_new(void)
{
	t_to_be_closed	*element;

	element = malloc(sizeof(t_to_be_closed));
	element->fds = malloc(sizeof(int) * 8);
	element->capacity = 8;
	element->size = 0;
	return (element);
}

void	to_be_closed_add(t_to_be_closed	*element, int fd)
{
	int		*new_fds;
	size_t	new_capacity;

	if (element->size + 1 >= element->capacity)
	{
		new_capacity = (element->size + 1) << 1;
		new_fds = malloc(sizeof(int) * new_capacity);
		if (new_fds == NULL)
			memory_error();
		bytes_copy(new_fds, element->fds,
			element->size * sizeof(int));
		free(element->fds);
		element->capacity = new_capacity;
		element->fds = new_fds;
	}
	element->fds[element->size++] = fd;
}
