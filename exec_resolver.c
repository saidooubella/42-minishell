#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#include "string_builder.h"
#include "exec_resolver.h"
#include "interpreter.h"
#include "string.h"

char	*error_msg(t_error_type err_type)
{
	if (err_type == NOT_FOUND)
		return ("No such file or directory");
	if (err_type == PERMISSION_DENIED)
		return ("Permission denied");
	error("Illegal state in 'error_msg'");
	return (NULL);
}

t_resolve_result	result_error(t_error_type err_type)
{
	t_resolve_result	result;
	
	result.executable_path = string_optional(string_create(NULL, false), false);
	result.err_type = err_type;
	result.success = false;
	return (result);
}

t_resolve_result	result_success(t_string executable_path)
{
	t_resolve_result	result;

	result.executable_path = string_optional(executable_path, true);
	result.err_type = NONE;
	result.success = true;
	return (result);
}

static char	*build_path(
	t_string_builder *b, char *path, char *cmd, size_t cmd_size)
{
	string_builder_append_string(b, path, string_length(path));
	string_builder_append_char(b, '/');
	string_builder_append_string(b, cmd, cmd_size);
	return (string_builder_to_cstr(b));
}

char	*find_command_path(char **paths, char *command, t_error_type *error)
{
	t_string_builder	*builder;
	size_t				command_length;
	char				*full_path;

	builder = string_builder_new();
	command_length = string_length(command);
	full_path = NULL;
	*error = NONE;
	while (*paths)
	{
		full_path = build_path(builder, *paths, command, command_length);
		if (access(full_path, F_OK) == 0)
		{
			if (access(full_path, X_OK) != 0)
				*error = PERMISSION_DENIED;
			break ;
		}
		string_builder_clear(builder);
		free(full_path);
		full_path = NULL;
		paths++;
	}
	string_builder_free(&builder);
	if (full_path == NULL)
		*error = NOT_FOUND;
	if (*error != NONE && full_path != NULL)
	{
		free(full_path);
		full_path = NULL;
	}
	return (full_path);
}

t_resolve_result	resolve_executable(t_environment *env, char *executable)
{
	if (string_index_of(executable, "/") != -1)
	{
		if (access(executable, F_OK))
			return (result_error(NOT_FOUND));
		if (access(executable, X_OK))
			return (result_error(PERMISSION_DENIED));
		return (result_success(string_create(executable, false)));
	}
	char *path = env_get_var(env, "PATH", NULL);
	if (path == NULL)
		return (result_error(NOT_FOUND));
	char **paths = string_split(path, ":");
	t_error_type	err_type;
	char *actual_path = find_command_path(paths, executable, &err_type);
	char **temp = paths;
	while (*temp)
		free(*temp++);
	free(paths);
	if (actual_path == NULL)
		return (result_error(err_type));
	return (result_success(string_create(actual_path, true)));
}
