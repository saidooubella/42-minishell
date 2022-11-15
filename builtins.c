#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#include "string_utils.h"
#include "lexer_utils.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "parser.h"

// TODO - Check for repetitive flags
int	echo_builtin(t_environment *env, size_t argc, t_string *argv)
{
	bool	new_line;
	bool	loop;
	size_t	start;
	size_t	cont;
	size_t	index;

	(void) env;
	new_line = true;
	loop = true;
	start = 0;
	while (loop && ++start < argc)
	{
		if (!string_starts_with(argv[start].value, "-", 1)
			|| string_equals(argv[start].value, "-"))
			break;
		index = 0;
		while (argv[start].value[++index])
			if (argv[start].value[index] != 'n')
				goto out;
		new_line = false;
	}
	out:
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
	t_string	target;

	if (argc < 2)
	{
		target = string_create(env_get_var(env, "HOME", NULL), false);
		if (target.value == NULL)
		{
			ft_printf(STDERR_FILENO, "cd: HOME not set\n");
			return (1);
		}
	}
	else if (string_equals(argv[1].value, "-"))
	{
		target = string_create(env_get_var(env, "OLDPWD", NULL), false);
		if (target.value == NULL)
		{
			ft_printf(STDERR_FILENO, "cd: OLDPWD not set\n");
			return (1);
		}
	}
	else
	{
		target = string_create(argv[1].value, false);
		if (access(target.value, F_OK) != 0)
		{
			ft_printf(STDERR_FILENO, "cd: no such file or directory: %s\n", target);
			return (1);
		}
		char *x = string_join(env->working_dir.value, "/");
		char *y = string_join(x, target.value);
		target = string_create(y, true);
		free(x);
	}
	t_symbol	symbol;
	symbol.name = string_create("OLDPWD", false);
	symbol.value = env->working_dir;
	env_put_var(env, symbol);
	chdir(target.value);
	string_free(&target);
	target = string_create(getcwd(NULL, 0), true);
	if (target.value == NULL)
	{
		ft_printf(STDERR_FILENO, "cd: can't retreive the working directory");
		return (1);
	}
	env->working_dir = target;
	return (0);
}

int	pwd_builtin(t_environment *env, size_t argc, t_string *argv)
{
	(void) argc;
	(void) argv;
	ft_printf(STDOUT_FILENO, "%s\n", env->working_dir.value);
	return (0);
}

int	export_builtin(t_environment *env, size_t argc, t_string *argv)
{
	t_symbol	symbol;
	size_t		index;

	if (argc <= 1)
	{
		index = 0;
		while (++index < env->symbols_size)
		{
			symbol = env->symbols[index];
			ft_printf(STDOUT_FILENO, "declare -x %s=%s\n", symbol.name.value, symbol.value.value);
		}
		return (0);
	}
	index = 0;
	while (++index < argc)
	{
		char *arg = argv[index].value;
		size_t index = 0;
		if (is_identifier_start(arg[index]))
			while (is_identifier_cont(arg[index]))
				index++;
		if (arg[index + 1] == '\0')
			continue;
		if (index == 0)
		{
			ft_printf(STDERR_FILENO, "export: '%s': not a valid identifier\n", arg);
			continue;
		}
		if (string_starts_with(arg + index, "=", 1))
		{
			symbol.name = string_create(substring(arg, 0, index), true);
			symbol.value = string_create(substring(arg, index + 1, string_length(arg)), true);
		}
		else if (string_starts_with(arg + index, "+=", 2))
		{
			symbol.name = string_create(substring(arg, 0, index), true);
			char *value = substring(arg, index + 2, string_length(arg));
			char *joined = string_join(env_get_var(env, symbol.name.value, ""), value);
			symbol.value = (free(value), string_create(joined, true));
		}
		else
		{
			ft_printf(STDERR_FILENO, "export: `%s': not a valid identifier\n", arg);
			continue;
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
	{
		char *arg = argv[index].value;
		size_t jndex = 0;
		if (is_identifier_start(arg[jndex]))
			while (is_identifier_cont(arg[jndex]))
				jndex++;
		if (arg[jndex] != '\0')
		{
			ft_printf(STDERR_FILENO, "export: `%s': not a valid identifier\n", arg);
			continue ;
		}
		env_remove_var(env, arg);
	}
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
