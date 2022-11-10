#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#include "string_utils.h"
#include "lexer_utils.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "parser.h"

int	echo_builtin(t_environment *env, size_t argc, t_string *argv)
{
	bool	new_line;
	size_t	start;
	size_t	cont;

	(void) env;
	new_line = true;
	start = 0;
	while (++start < argc)
	{
		if (!string_equals(argv[start].value, "-n"))
			break;
		new_line = false;
	}
	cont = start - 1;
	while (++cont < argc)
	{
		if (cont > start)
			ft_printf(STDOUT_FILENO, " ");
		ft_printf(STDOUT_FILENO, "%s", argv[cont].value);
	}
	if (new_line)
		ft_printf(STDOUT_FILENO, "\n");
	return (0);
}

int	cd_builtin(t_environment *env, size_t argc, t_string *argv)
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
		target = argv[1].value;
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

int	pwd_builtin(t_environment *env, size_t argc, t_string *argv)
{
	(void) argc;
	(void) argv;
	ft_printf(STDOUT_FILENO, "%s\n", env->working_dir.value);
	return (0);
}

static ssize_t	string_index_of(char *string, char target)
{
	ssize_t	index;

	index = 0;
	while (string[index])
	{
		if (string[index] == target)
			return (index);
		index++;
	}
	return (-1);
}

static bool	validate_name(char *identifier)
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

int	export_builtin(t_environment *env, size_t argc, t_string *argv)
{
	t_symbol	symbol;
	ssize_t		result;
	size_t		index;

	index = 0;
	while (++index < argc)
	{
		result = string_index_of(argv[index].value, '=');
		if (result != -1 && argv[index].value[0] != '=')
		{
			symbol.name = string_create(substring(argv[index].value, 0, result), true);
			symbol.value = string_create(substring(argv[index].value, result + 1, string_length(argv[index].value)), true);
		}
		else
		{
			symbol.name = string_create(argv[index].value, false);
			symbol.value = string_create(NULL, false);
		}
		if (!validate_name(symbol.name.value))
		{
			ft_printf(STDERR_FILENO, "export: '%s': not a valid identifier\n", argv[index].value);
			string_free(&symbol.value);
			string_free(&symbol.name);
			return (1);
		}
		env_put_var(env, symbol);
	}
	return (0);
}

int	unset_builtin(t_environment *env, size_t argc, t_string *argv)
{
	size_t	index;

	index = 0;
	while (++index < argc)
		env_remove_var(env, argv[index].value);
	return (0);
}

int	env_builtin(t_environment *env, size_t argc, t_string *argv)
{
	size_t		index;
	t_symbol	symbol;

	(void) argv;
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

int	exit_builtin(t_environment *env, size_t argc, t_string *argv)
{
	(void) argc;
	(void) argv;
	env->running = false;
	return (0);
}
