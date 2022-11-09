/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 14:38:21 by soubella          #+#    #+#             */
/*   Updated: 2022/11/09 14:41:16 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "string.h"

t_string	string_create(char *value, bool freeable)
{
	t_string	string;

	string.freeable = freeable;
	string.value = value;
	return (string);
}

void	string_free(t_string *string)
{
	if (string->freeable)
		free(string->value);
	string->value = NULL;
}
