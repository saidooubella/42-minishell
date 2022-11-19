/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:37:37 by soubella          #+#    #+#             */
/*   Updated: 2022/11/19 14:23:45 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#include "string_utils.h"
#include "lexer_utils.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "parser.h"

t_builtin	*initilize_builtins(void)
{
	t_builtin	*builtins;
	size_t		index;

	builtins = malloc(sizeof(t_builtin) * BUILTINS_COUNT);
	if (builtins == NULL)
		memory_error();
	index = -1;
	builtins[++index].name = "echo";
	builtins[index].block = echo_builtin;
	builtins[++index].name = "cd";
	builtins[index].block = cd_builtin;
	builtins[++index].name = "pwd";
	builtins[index].block = pwd_builtin;
	builtins[++index].name = "export";
	builtins[index].block = export_builtin;
	builtins[++index].name = "unset";
	builtins[index].block = unset_builtin;
	builtins[++index].name = "exit";
	builtins[index].block = exit_builtin;
	builtins[++index].name = "env";
	builtins[index].block = env_builtin;
	return (builtins);
}

t_builtin	*builtin_lookup(t_builtin *builtins, char *target)
{
	size_t		index;

	index = -1;
	while (++index < BUILTINS_COUNT)
		if (string_equals(builtins[index].name, target))
			return (&builtins[index]);
	return (NULL);
}

int	echo_builtin(t_environment *env, size_t argc, char **argv)
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
		char *arg = argv[start];
		if (!string_starts_with(arg, "-", 1)
			|| string_equals(arg, "-"))
			break;
		index = 0;
		while (arg[++index])
		{
			if (arg[index] != 'n')
			{
				loop = false;
				break ;
			}
		}
		if (loop)
			new_line = false;
	}
	cont = start - 1;
	while (++cont < argc)
	{
		if (cont > start)
			ft_printf(STDOUT_FILENO, " ");
		ft_printf(STDOUT_FILENO, "%s", argv[cont]);
	}
	if (new_line)
		ft_printf(STDOUT_FILENO, "\n");
	return (0);
}

int	cd_builtin(t_environment *env, size_t argc, char **argv)
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
	else
	{
		char *arg = argv[1];
		if (string_equals(arg, "-"))
		{
			target = string_create(env_get_var(env, "OLDPWD", NULL), false);
			if (target.value == NULL)
			{
				ft_printf(STDERR_FILENO, "cd: OLDPWD not set\n");
				return (1);
			}
			ft_printf(STDOUT_FILENO, "%s", target.value);
		}
		else
		{
			if (string_starts_with(arg, "/", 1))
				target = string_create(arg, false);
			else
			{
				char *x = string_join(env->working_dir.value, "/");
				char *y = string_join(x, arg);
				target = (free(x), string_create(y, true));
			}
			ft_printf(STDOUT_FILENO, ">> %s\n", target.value);
			if (access(target.value, F_OK) != 0)
				ft_printf(STDERR_FILENO, "cd: no such file or directory: %s\n", arg);
		}
	}
	t_symbol	symbol;
	symbol.name = string_create("OLDPWD", false);
	symbol.value = env->working_dir;
	env_put_var(env, symbol, true);
	if (access(target.value, F_OK) == 0)
	{
		chdir(target.value);
		string_free(&target);
		target = string_create(getcwd(NULL, 0), true);
		ft_printf(STDOUT_FILENO, ">> %s\n", target.value);
	}
	if (target.value == NULL)
	{
		ft_printf(STDERR_FILENO, "cd: can't retreive the working directory\n");
		return (1);
	}
	env->working_dir = target;
	symbol.name = string_create("PWD", false);
	symbol.value = string_create(env->working_dir.value, false);
	env_put_var(env, symbol, true);
	return (0);
}

int	pwd_builtin(t_environment *env, size_t argc, char **argv)
{
	(void) argc;
	(void) argv;
	ft_printf(STDOUT_FILENO, "%s\n", env->working_dir.value);
	return (0);
}

int	export_builtin(t_environment *env, size_t argc, char **argv)
{
	t_symbol	symbol;
	size_t		index;

	if (argc <= 1)
	{
		index = -1;
		while (++index < env->symbols_size)
		{
			symbol = env->symbols[index];
			ft_printf(STDOUT_FILENO, "declare -x %s=", symbol.name.value);
			if (symbol.value.value != NULL)
				ft_printf(STDOUT_FILENO, "%s", symbol.value.value);
			ft_printf(STDOUT_FILENO, "\n");
		}
		return (0);
	}
	index = 0;
	while (++index < argc)
	{
		char *arg = argv[index];
		size_t index = 0;
		if (is_identifier_start(arg[index]))
			while (is_identifier_cont(arg[index]))
				index++;
		if (arg[index] == '\0')
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
		env_put_var(env, symbol, false);
	}
	return (0);
}

int	unset_builtin(t_environment *env, size_t argc, char **argv)
{
	size_t	index;

	index = 0;
	while (++index < argc)
	{
		char *arg = argv[index];
		size_t jndex = 0;
		if (is_identifier_start(arg[jndex]))
			while (is_identifier_cont(arg[jndex]))
				jndex++;
		if (arg[jndex] != '\0')
		{
			ft_printf(STDERR_FILENO, "export: `%s': not a valid identifier\n", arg);
			return (1);
		}
		env_remove_var(env, arg);
	}
	return (0);
}

int	env_builtin(t_environment *env, size_t argc, char **argv)
{
	size_t		index;
	t_symbol	symbol;

	(void) argv;
	if (argc > 1)
	{
		ft_printf(STDERR_FILENO, "env: too many arguments\n");
		return (1);
	}
	index = -1;
	while (++index < env->symbols_size)
	{
		symbol = env->symbols[index];
		ft_printf(STDOUT_FILENO, "%s=", symbol.name.value);
		if (symbol.value.value != NULL)
			ft_printf(STDOUT_FILENO, "%s", symbol.value.value);
		ft_printf(STDOUT_FILENO, "\n");
	}
	return (0);
}

int	exit_builtin(t_environment *env, size_t argc, char **argv)
{
	bool		error;
	long long	exit_code;

	(void) env;
	(void) argv;
	if (argc <= 1)
	{
		ft_printf(STDOUT_FILENO, "exit\n");
		exit(0);
	}
	char *arg = argv[1];
	exit_code = string_to_llong(arg, &error);
	ft_printf(STDOUT_FILENO, "exit\n", arg);
	if (error)
	{
		exit_code = 255;
		ft_printf(STDERR_FILENO, "exit: %s: numeric argument required\n", arg);
	}
	else if (argc > 2)
	{
		ft_printf(STDERR_FILENO, "exit: too many arguments\n");
		return (1);
	}
	else
	{
		exit_code = 255;
		ft_printf(STDERR_FILENO, "exit: %s: numeric argument required\n", arg);
	}
	exit(exit_code & 0xFF);
}
