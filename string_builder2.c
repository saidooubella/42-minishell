/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 11:33:26 by soubella          #+#    #+#             */
/*   Updated: 2022/11/14 15:48:02 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "string_builder.h"
#include "string_utils.h"

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
