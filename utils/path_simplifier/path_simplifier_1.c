/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_simplifier_1.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 21:45:57 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:09:22 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>

#include "path_simplifier.h"
#include "string_builder.h"
#include "string_utils.h"
#include "utils.h"

void	remove_last(t_path **_path)
{
	t_path	*path;

	path = *_path;
	while (path && path->next && path->next->next)
		path = path->next;
	if (path == 0)
		return ;
	if (path->next == 0)
	{
		path_free(path);
		*_path = 0;
	}
	else
	{
		path_free(path->next);
		path->next = 0;
	}
}

void	add_last(t_path **_path, t_path *new)
{
	t_path	*path;

	path = *_path;
	if (path == 0)
	{
		*_path = new;
		return ;
	}
	while (path && path->next)
		path = path->next;
	if (path == 0)
	{
		path_free(new);
		return ;
	}
	path->next = new;
}

t_path	*path_simplify(t_path *path)
{
	t_path	*head;

	head = NULL;
	while (path)
	{
		if (string_equals(path->part, ".."))
			remove_last(&head);
		else if (!string_equals(path->part, "."))
			add_last(&head, path_new(string_duplicate(path->part)));
		path = path->next;
	}
	return (head);
}

char	*path_to_string(t_path *path)
{
	t_string_builder	*builder;
	char				*result;

	builder = string_builder_new();
	result = 0;
	while (path)
	{
		string_builder_append_cstring(builder, path->part);
		string_builder_append_char(builder, '/');
		path = path->next;
	}
	result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	return (result);
}

char	*simplify_path(char *path)
{
	t_path	*simple_path;
	t_path	*in_path;
	char	*result;

	in_path = path_from_string(path);
	simple_path = path_simplify(in_path);
	result = path_to_string(simple_path);
	path_free(simple_path);
	path_free(in_path);
	return (result);
}
