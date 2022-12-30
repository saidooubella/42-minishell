/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   element.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 13:08:30 by soubella          #+#    #+#             */
/*   Updated: 2022/12/30 12:56:45 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ELEMENT_H
# define ELEMENT_H

# include <stddef.h>

# include "free_aware_string.h"
# include "string_builder.h"

typedef struct s_environment	t_environment;

typedef enum e_element_type
{
	WORD_ELEMENT,
	VAR_ELEMENT,
}	t_element_type;

typedef struct s_element
{
	t_element_type	type;
	t_string		value;
	bool			in_string;
	bool			expandable;
}	t_element;

typedef struct s_elements
{
	bool			expandable;
	t_element		*elements;
	size_t			capacity;
	size_t			size;
}	t_elements;

typedef struct s_optional_elements
{
	t_elements	*elements;
	bool		present;
}	t_optional_elements;

void				handle_variable(
						t_environment *env, t_string_builder *builder,
						t_elements *elements, size_t index);

t_element			element_create(t_string value, t_element_type type,
						bool expandable, bool in_string);
t_string			elements_resolve(t_elements *elements, t_environment *env);
t_optional_elements	elements_optional(t_elements *elements, bool present);
void				elements_add(t_elements *elements, t_element element);
void				elements_fill(t_elements *dst, t_elements **src);
void				elements_free(t_elements **elements);
t_elements			*elements_new_cap(size_t capacity);
t_elements			*elements_new(void);

#endif
