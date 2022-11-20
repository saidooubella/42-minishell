#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>

#include "path_simplifier.h"
#include "string_builder.h"
#include "string_utils.h"

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

t_path	*path_from_string(char *path)
{
	size_t	path_len;
	size_t	offset;
	t_path	*head;
	t_path	*tail;
	ssize_t	index;
	t_path	**target;
	t_path	*new;
	char	*sub;

	path_len = string_length(path);
	offset = 0;
	head = NULL;
	tail = NULL;
	while (offset < path_len)
	{
		index = string_index_of(path + offset, "/");
		if (index == -1)
			index = string_length(path + offset);
		sub = substring(path + offset, 0, index);
		offset += index + 1;
		target = &head;
		if (head != 0)
			target = &tail->next;
		new = malloc(sizeof(t_path));
		new->part = sub;
		new->next = 0;
		*target = new;
		tail = new;
	}
	return head;
}

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
		else if (!string_equals(path->part, ".")) {
			t_path *new = malloc(sizeof(t_path));
			new->part = string_duplicate(path->part);
			new->next = 0;
			add_last(&head, new);
		}
		path = path->next;
	}
	return head;
}

char	*path_to_string(t_path *path)
{
	t_string_builder	*builder;
	char 				*result;

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
	return result;
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
	return result;
}
