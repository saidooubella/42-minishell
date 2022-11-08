/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:02:00 by soubella          #+#    #+#             */
/*   Updated: 2022/11/08 10:07:48 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_BUILDER_H
# define STRING_BUILDER_H

# include <sys/types.h>

# define DEFAULT_CAPACITY 16

typedef struct s_string_builder
{
	char	*buffer;
	size_t	capacity;
	size_t	size;
}	t_string_builder;

t_string_builder	*string_builder_new_cap(size_t capacity);
t_string_builder	*string_builder_new(void);
void				string_builder_clear(t_string_builder *b);
void				string_builder_free(t_string_builder **b);
int					string_builder_append_string(
						t_string_builder *b, const char *str, size_t size);
int					string_builder_append_cstring(
						t_string_builder *b, const char *str);
int					string_builder_append_char(t_string_builder *b, char chr);
char				*string_builder_to_cstr(t_string_builder *b);	

#endif
