/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   element_2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:26:10 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 15:35:10 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "free_aware_string.h"
#include "string_utils.h"
#include "element.h"
#include "utils.h"

t_elements	*elements_new_cap(size_t capacity)
{
	t_elements	*elements;

	elements = malloc(sizeof(t_elements));
	if (elements == NULL)
		memory_error();
	elements->elements = malloc(sizeof(t_element) * capacity);
	if (elements->elements == NULL)
		memory_error();
	elements->capacity = capacity;
	elements->size = 0;
	return (elements);
}

void	elements_fill(t_elements *dst, t_elements **src)
{
	t_element	*temp;
	size_t		index;

	index = 0;
	while (index < (*src)->size)
	{
		temp = &(*src)->elements[index++];
		elements_add(dst, element_create(temp->value, temp->type,
				temp->expandable, temp->in_string));
	}
	free((*src)->elements);
	free((*src));
	*src = NULL;
}

t_element	element_create(
	t_string value, t_element_type type, bool expandable, bool in_string)
{
	t_element	element;

	element.expandable = expandable;
	element.expandable = expandable;
	element.in_string = in_string;
	element.value = value;
	element.type = type;
	return (element);
}

void	elements_add(t_elements *elements, t_element element)
{
	size_t		new_capacity;
	t_element	*new_elements;

	if (elements->size + 1 >= elements->capacity)
	{
		new_capacity = (elements->size + 1) << 1;
		new_elements = malloc(sizeof(t_element) * new_capacity);
		if (new_elements == NULL)
			memory_error();
		bytes_copy(new_elements, elements->elements,
			elements->size * sizeof(t_element));
		free(elements->elements);
		elements->capacity = new_capacity;
		elements->elements = new_elements;
	}
	elements->elements[elements->size++] = element;
}

void	elements_free(t_elements **elements)
{
	size_t	index;

	index = -1;
	while (++index < (*elements)->size)
		string_free(&(*elements)->elements[index].value);
	free((*elements)->elements);
	free((*elements));
	*elements = NULL;
}
