/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_resolver.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:37:45 by soubella          #+#    #+#             */
/*   Updated: 2022/11/16 11:09:08 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#include "string_builder.h"
#include "exec_resolver.h"
#include "interpreter.h"
#include "string.h"

static char	*build_path(
	t_string_builder *b, char *path, char *cmd, size_t cmd_size)
{
	string_builder_append_string(b, path, string_length(path));
	string_builder_append_char(b, '/');
	string_builder_append_string(b, cmd, cmd_size);
	return (string_builder_to_cstr(b));
}

char	*find_command_path(char **paths, char *command)
{
	t_string_builder	*builder;
	size_t				command_length;
	char				*full_path;

	builder = string_builder_new();
	command_length = string_length(command);
	full_path = NULL;
	while (*paths)
	{
		full_path = build_path(builder, *paths, command, command_length);
		if (access(full_path, F_OK) == 0)
			break ;
		string_builder_clear(builder);
		free(full_path);
		full_path = NULL;
		paths++;
	}
	string_builder_free(&builder);
	return (full_path);
}

char	*resolve_executable(t_environment *env, char *executable)
{
	if (string_index_of(executable, "/") != -1)
		return (executable);
	char *path = env_get_var(env, "PATH", NULL);
	if (path == NULL)
		return (NULL);
	char **paths = string_split(path, ":");
	char *actual_path = find_command_path(paths, executable);
	char **temp = paths;
	while (*temp)
		free(*temp++);
	free(paths);
	if (actual_path == NULL)
		return (NULL);
	return (actual_path);
}
