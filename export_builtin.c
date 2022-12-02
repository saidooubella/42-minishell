/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_builtin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:17:55 by soubella          #+#    #+#             */
/*   Updated: 2022/12/02 15:54:27 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "string_utils.h"
#include "environment.h"
#include "lexer_utils.h"
#include "ft_printf.h"

static int	export_print_env(t_environment *env)
{
	t_symbol	symbol;
	size_t		index;

	index = -1;
	while (++index < env->symbols_size)
	{
		symbol = env->symbols[index];
		ft_printf(STDOUT_FILENO, "declare -x %s", symbol.name.value);
		if (symbol.has_value)
			ft_printf(STDOUT_FILENO, "=");
		if (symbol.value.value != NULL)
			ft_printf(STDOUT_FILENO, "%s", symbol.value.value);
		ft_printf(STDOUT_FILENO, "\n");
	}
	return (0);
}

static size_t	export_skip_name(char *arg)
{
	size_t	index;

	index = 0;
	if (is_identifier_start(arg[index]))
		while (is_identifier_cont(arg[index]))
			index++;
	return (index);
}

static void	export_handle_arg(t_environment *env, char *arg, size_t index)
{
	t_symbol	symbol;
	char		*value;
	char		*joined;

	if (string_starts_with(arg + index, "=", 1))
	{
		symbol.name = string_create(substring(arg, 0, index), true);
		symbol.value = string_create(
				substring(arg, index + 1, string_length(arg)), true);
	}
	else if (string_starts_with(arg + index, "+=", 2))
	{
		symbol.name = string_create(substring(arg, 0, index), true);
		value = substring(arg, index + 2, string_length(arg));
		joined = string_join(env_get_var(env, symbol.name.value, ""), value);
		symbol.value = (free(value), string_create(joined, true));
	}
	else
	{
		ft_printf(STDERR_FILENO, "export: `%s': not a valid identifier\n", arg);
		return ;
	}
	env_put_var(env, symbol, false, true);
}

int	export_builtin(t_environment *env, size_t argc, char **argv)
{
	t_symbol	symbol;
	size_t		index;
	size_t		jndex;
	char		*arg;

	if (argc <= 1)
		return (export_print_env(env));
	index = 0;
	while (++index < argc)
	{
		arg = argv[index];
		jndex = export_skip_name(arg);
		if (arg[jndex] == '\0')
		{
			symbol.name = string_create(string_duplicate(arg), true);
			symbol.value = string_create(NULL, false);
			env_put_var(env, symbol, false, false);
		}
		else if (jndex == 0)
			ft_printf(STDERR_FILENO,
				"export: '%s': not a valid identifier\n", arg);
		else
			export_handle_arg(env, arg, jndex);
	}
	return (0);
}
