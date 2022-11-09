#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#include "string_utils.h"
#include "lexer_utils.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "parser.h"

int	echo_builtin(t_environment *env, int argc, char **argv)
{
	bool	no_new_line;
	size_t	index;

	(void) env;
	no_new_line = false;
	index = 0;
	while (++index < argc)
	{
		if (!string_equals(argv[index], "-n"))
			break;
		no_new_line = true;
	}
	while (++index < argc)
		ft_printf(STDOUT_FILENO, "%s", argv[index]);
	if (!no_new_line)
		ft_printf(STDOUT_FILENO, "\n");
	return (0);
}

int	cd_builtin(t_environment *env, int argc, char **argv)
{
	char	*target;

	if (argc < 2)
	{
		target = env_get_var(env, "HOME", NULL);
		if (target == NULL)
		{
			ft_printf(STDERR_FILENO, "cd: HOME not set\n");
			return (1);
		}
	}
	else
	{
		target = argv + 1;
		if (access(target, F_OK) != 0)
		{
			ft_printf(STDERR_FILENO, "cd: no such file or directory: %s\n", target);
			return (1);
		}
	}
	string_free(&env->working_dir);
	env->working_dir = string_create(target, false);
	return (0);
}

int	pwd_builtin(t_environment *env, int argc, char **argv)
{
	ft_printf(STDOUT_FILENO, "%s\n", env->working_dir.value);
	return (0);
}

size_t	string_index_of(char *string, char target)
{
	size_t	index;

	index = 0;
	while (string[index])
	{
		if (string[index] == target)
			return (index);
		index++;
	}
	return (-1);
}

bool	validate_name(char *identifier)
{
	size_t	length;

	length = string_length(identifier);
	if (length == 0)
		return (false);
	if (!is_identifier_start(identifier[0]))
		return (false);
	while (*identifier)
		if (!is_identifier_cont(*identifier++))
			return (false);
	return (true);
}

int	export_builtin(t_environment *env, int argc, char **argv)
{
	t_symbol	symbol;
	size_t		result;
	size_t		index;

	index = 0;
	while (++index < argc)
	{
		result = string_index_of(argv[index], '=');
		if (result != -1)
		{
			symbol.name = string_create(substring(argv[index], 0, result), true);
			symbol.value = string_create(substring(argv[index], result + 1, string_length(argv[index])), true);
		}
		else
		{
			symbol.name = string_create(argv[index], false);
			symbol.value = string_create(NULL, false);
		}
		if (!validate_name(symbol.name.value))
		{
			ft_printf(STDERR_FILENO, "export: '%s': not a valid identifier\n", symbol.name.value);
			return (1);
		}
		env_put_var(env, symbol);
	}
	return (0);
}

int	unset_builtin(t_environment *env, int argc, char **argv)
{
	size_t	index;

	index = 0;
	while (++index < argc)
		env_remove_var(env, argv[index]);
	return (0);
}

int	env_builtin(t_environment *env, int argc, char **argv)
{
	size_t		index;
	t_symbol	symbol;

	if (argc > 1)
	{
		ft_printf(STDERR_FILENO, "env: too many arguments\n");
		return (1);
	}
	index = 0;
	while (++index < env->symbols_size)
	{
		symbol = env->symbols[index];
		ft_printf(STDOUT_FILENO, "%s=%s\n", symbol.name.value, symbol.value.value);
	}
	return (0);
}

int	exit_builtin(t_environment *env, int argc, char **argv)
{
	env->running = false;
	return (0);
}
