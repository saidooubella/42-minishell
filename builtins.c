/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:37:37 by soubella          #+#    #+#             */
/*   Updated: 2022/11/18 15:40:12 by soubella         ###   ########.fr       */
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

int	echo_builtin(t_environment *env, size_t argc, t_elements **argv)
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
		t_string arg = elements_resolve(argv[start], env);
		if (!string_starts_with(arg.value, "-", 1)
			|| string_equals(arg.value, "-"))
		{
			string_free(&arg);
			break;
		}
		index = 0;
		while (arg.value[++index])
		{
			if (arg.value[index] != 'n')
			{
				loop = false;
				string_free(&arg);
				break ;
			}
		}
		if (loop)
			new_line = false;
		string_free(&arg);
	}
	cont = start - 1;
	while (++cont < argc)
	{
		t_string arg = elements_resolve(argv[cont], env);
		if (cont > start)
			ft_printf(STDOUT_FILENO, " ");
		ft_printf(STDOUT_FILENO, "%s", arg.value);
		string_free(&arg);
	}
	if (new_line)
		ft_printf(STDOUT_FILENO, "\n");
	return (0);
}

int	cd_builtin(t_environment *env, size_t argc, t_elements **argv)
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
		t_string arg = elements_resolve(argv[1], env);
		if (string_equals(arg.value, "-"))
		{
			target = string_create(env_get_var(env, "OLDPWD", NULL), false);
			if (target.value == NULL)
			{
				ft_printf(STDERR_FILENO, "cd: OLDPWD not set\n");
				string_free(&arg);
				return (1);
			}
			ft_printf(STDOUT_FILENO, "%s", target.value);
		}
		else
		{
			if (string_starts_with(arg.value, "/", 1))
				target = string_create(arg.value, false);
			else
			{
				char *x = string_join(env->working_dir.value, "/");
				char *y = string_join(x, arg.value);
				target = (free(x), string_create(y, true));
			}
			ft_printf(STDOUT_FILENO, ">> %s\n", target.value);
			if (access(target.value, F_OK) != 0)
				ft_printf(STDERR_FILENO, "cd: no such file or directory: %s\n", arg.value);
		}
		string_free(&arg);
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

int	pwd_builtin(t_environment *env, size_t argc, t_elements **argv)
{
	(void) argc;
	(void) argv;
	ft_printf(STDOUT_FILENO, "%s\n", env->working_dir.value);
	return (0);
}

int	export_builtin(t_environment *env, size_t argc, t_elements **argv)
{
	t_symbol	symbol;
	size_t		index;

	if (argc <= 1)
	{
		index = -1;
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
		t_string arg = elements_resolve(argv[index], env);
		size_t index = 0;
		if (is_identifier_start(arg.value[index]))
			while (is_identifier_cont(arg.value[index]))
				index++;
		if (arg.value[index] == '\0')
		{
			string_free(&arg);
			continue;
		}
		if (index == 0)
		{
			ft_printf(STDERR_FILENO, "export: '%s': not a valid identifier\n", arg.value);
			string_free(&arg);
			continue;
		}
		if (string_starts_with(arg.value + index, "=", 1))
		{
			symbol.name = string_create(substring(arg.value, 0, index), true);
			symbol.value = string_create(substring(arg.value, index + 1, string_length(arg.value)), true);
		}
		else if (string_starts_with(arg.value + index, "+=", 2))
		{
			symbol.name = string_create(substring(arg.value, 0, index), true);
			char *value = substring(arg.value, index + 2, string_length(arg.value));
			char *joined = string_join(env_get_var(env, symbol.name.value, ""), value);
			symbol.value = (free(value), string_create(joined, true));
		}
		else
		{
			ft_printf(STDERR_FILENO, "export: `%s': not a valid identifier\n", arg);
			string_free(&arg);
			continue;
		}
		env_put_var(env, symbol, false);
		string_free(&arg);
	}
	return (0);
}

int	unset_builtin(t_environment *env, size_t argc, t_elements **argv)
{
	size_t	index;

	index = 0;
	while (++index < argc)
	{
		t_string arg = elements_resolve(argv[index], env);
		size_t jndex = 0;
		if (is_identifier_start(arg.value[jndex]))
			while (is_identifier_cont(arg.value[jndex]))
				jndex++;
		if (arg.value[jndex] != '\0')
		{
			ft_printf(STDERR_FILENO, "export: `%s': not a valid identifier\n", arg.value);
			string_free(&arg);
			return (1);
		}
		env_remove_var(env, arg.value);
		string_free(&arg);
	}
	return (0);
}

int	env_builtin(t_environment *env, size_t argc, t_elements **argv)
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
		ft_printf(STDOUT_FILENO, "%s=%s\n", symbol.name.value, symbol.value.value);
	}
	return (0);
}

int	exit_builtin(t_environment *env, size_t argc, t_elements **argv)
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
	t_string arg = elements_resolve(argv[1], env);
	exit_code = string_to_llong(arg.value, &error);
	ft_printf(STDOUT_FILENO, "exit\n", arg.value);
	if (error)
	{
		exit_code = 255;
		ft_printf(STDERR_FILENO, "exit: %s: numeric argument required\n", arg.value);
	}
	else if (argc > 2)
	{
		ft_printf(STDERR_FILENO, "exit: too many arguments\n");
		string_free(&arg);
		return (1);
	}
	else
	{
		exit_code = 255;
		ft_printf(STDERR_FILENO, "exit: %s: numeric argument required\n", arg.value);
	}
	string_free(&arg);
	exit(exit_code & 0xFF);
}
