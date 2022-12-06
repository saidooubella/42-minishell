/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_simplifier_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:09:25 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:11:31 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>

#include "path_simplifier.h"
#include "string_utils.h"
#include "utils.h"

void	path_free(t_path *path)
{
	t_path	*tmp;

	while (path)
	{
		tmp = path->next;
		free(path->part);
		free(path);
		path = tmp;
	}
}

t_path	*path_new(char *part)
{
	t_path	*path;

	path = malloc(sizeof(t_path));
	if (path == NULL)
		memory_error();
	path->part = part;
	path->next = NULL;
	return (path);
}

t_path	*path_from_string(char *path)
{
	t_pfs_locals	locals;

	locals.path_len = string_length(path);
	locals.offset = 0;
	locals.head = NULL;
	locals.tail = NULL;
	while (locals.offset < locals.path_len)
	{
		locals.index = string_index_of(path + locals.offset, "/");
		if (locals.index == -1)
			locals.index = string_length(path + locals.offset);
		locals.sub = substring(path + locals.offset, 0, locals.index);
		locals.offset += locals.index + 1;
		locals.target = &locals.head;
		if (locals.head != 0)
			locals.target = &locals.tail->next;
		locals.tail = path_new(locals.sub);
		*locals.target = locals.tail;
	}
	return (locals.head);
}
