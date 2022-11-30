/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_7.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 21:04:25 by soubella          #+#    #+#             */
/*   Updated: 2022/11/30 21:04:54 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "string_utils.h"

bool	has_multiple_parts(char *string)
{
	char	**splitted;
	bool	result;
	size_t	size;

	splitted = string_split(string, " \t\v\n\r\f");
	result = false;
	size = 0;
	while (splitted[size])
		size++;
	if (size != 1)
		result = true;
	size = -1;
	while (splitted[++size])
		free(splitted[size]);
	free(splitted);
	return (result);
}
