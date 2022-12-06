/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 11:23:01 by soubella          #+#    #+#             */
/*   Updated: 2022/11/28 17:53:29 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "string_utils.h"
#include "ft_printf.h"
#include "environment.h"
#include "utils.h"

#define SHELL_LEVEL_WARING "minishell: warning: shell level (%z)\
 too high, resetting to 1\n"

void	initilize_defaults(t_environment *env)
{
	t_symbol	symbol;
	int64_t		shlvl;
	bool		error;

	symbol.name = string_create("PWD", false);
	symbol.value = string_create(env->working_dir.value, false);
	env_put_var(env, symbol, true, true);
	symbol.name = string_create("SHLVL", false);
	shlvl = string_to_llong(env_get_var(env, "SHLVL", ""), false, &error);
	if (error || shlvl > 1000000000)
	{
		if (shlvl > 1000000000)
			ft_printf(STDOUT_FILENO, SHELL_LEVEL_WARING, shlvl + 1);
		shlvl = 1;
	}
	else
		shlvl += 1;
	symbol.value = string_create(llong_to_string(shlvl), true);
	env_put_var(env, symbol, true, true);
	symbol.name = string_create("_", false);
	symbol.value = string_create("/usr/bin/env", false);
	env_put_var(env, symbol, true, true);
}

void	fill_environment(t_environment *environment, char **env)
{
	char		**splitted;
	t_symbol	symbol;

	while (*env)
	{
		splitted = string_split(*env++, "=");
		symbol.name = string_create(splitted[0], splitted[0] != NULL);
		symbol.value = string_create(splitted[1], splitted[1] != NULL);
		if (!string_equals(symbol.name.value, "OLDPWD"))
			env_put_var(environment, symbol, true, true);
		else
		{
			string_free(&symbol.name);
			string_free(&symbol.value);
		}
		free(splitted);
	}
}

t_environment	*env_new(char **env, bool is_bonus)
{
	t_environment	*environment;

	environment = malloc(sizeof(t_environment));
	if (environment == NULL)
		memory_error();
	environment->symbols = NULL;
	environment->exit_code = 0;
	environment->symbols_size = 0;
	environment->symbols_cap = 0;
	environment->is_bonus = is_bonus;
	environment->working_dir = string_create(getcwd(NULL, 0), true);
	environment->builtins = initilize_builtins();
	if (environment->working_dir.value == NULL)
		error("Error: Cannot retreive the current working directory");
	fill_environment(environment, env);
	initilize_defaults(environment);
	return (environment);
}

void	env_free(t_environment **env)
{
	size_t	index;

	index = -1;
	while (++index < (*env)->symbols_size)
	{
		string_free(&(*env)->symbols[index].name);
		string_free(&(*env)->symbols[index].value);
	}
	string_free(&(*env)->working_dir);
	free((*env)->builtins);
	free((*env)->symbols);
	free(*env);
	*env = NULL;
}

void	env_insert_var(t_environment *env, t_symbol symbol)
{
	t_symbol	*new_symbols;
	size_t		new_capacity;

	if (env->symbols_size + 1 >= env->symbols_cap)
	{
		new_capacity = (env->symbols_size + 1) << 1;
		new_symbols = malloc(sizeof(t_symbol) * new_capacity);
		if (new_symbols == NULL)
			memory_error();
		bytes_copy(new_symbols, env->symbols,
			env->symbols_size * sizeof(t_symbol));
		free(env->symbols);
		env->symbols_cap = new_capacity;
		env->symbols = new_symbols;
	}
	env->symbols[env->symbols_size] = symbol;
	env->symbols_size++;
}
