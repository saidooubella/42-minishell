/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 11:23:01 by soubella          #+#    #+#             */
/*   Updated: 2022/11/15 18:50:48 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "string_utils.h"
#include "ft_printf.h"
#include "environment.h"
#include "utils.h"

#define SHELL_LEVEL_WARING "minishell: warning: shell level (1000000000)\
 too high, resetting to 1\n"

void	initilize_defaults(t_environment *env)
{
	t_symbol	symbol;
	long long	shlvl;
	bool		error;

	symbol.name = string_create("PWD", false);
	symbol.value = string_create(env->working_dir.value, false);
	env_put_var(env, symbol);
	symbol.name = string_create("SHLVL", false);
	shlvl = string_to_llong(env_get_var(env, "SHLVL", ""), &error);
	if (error || shlvl > 1000000000)
	{
		if (shlvl > 1000000000)
			ft_printf(STDOUT_FILENO, SHELL_LEVEL_WARING);
		shlvl = 1;
	}
	else
		shlvl += 1;
	symbol.value = string_create(llong_to_string(shlvl), true);
	env_put_var(env, symbol);
	symbol.name = string_create("_", false);
	symbol.value = string_create("/usr/bin/env", false);
	env_put_var(env, symbol);
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
		env_put_var(environment, symbol);
		free(splitted);
	}
}

t_environment *env_new(char **env)
{
	t_environment	*environment;

	environment = malloc(sizeof(t_environment));
	if (environment == NULL)
		memory_error();
	environment->symbols = NULL;
	environment->exit_code = 0;
	environment->symbols_size = 0;
	environment->symbols_cap = 0;
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
		bytes_copy(new_symbols, env->symbols, env->symbols_size * sizeof(t_symbol));
		free(env->symbols);
		env->symbols_cap = new_capacity;
		env->symbols = new_symbols;
	}
	env->symbols[env->symbols_size] = symbol;
	env->symbols_size++;
}

char	*env_get_var(t_environment *env, char *name, char *fallback)
{
	size_t	target;
	size_t	index;

	target = -1;
	index = -1;
	while (++index < env->symbols_size)
		if (string_equals(env->symbols[index].name.value, name))
			return (env->symbols[index].value.value);
	return (fallback);
}

void	env_remove_var(t_environment *env, char *name)
{
	ssize_t	target;
	size_t	index;

	target = -1;
	index = -1;
	if (string_equals(name, "_"))
		return ;
	while (++index < env->symbols_size)
	{
		if (!string_equals(env->symbols[index].name.value, name))
			continue ;
		target = index;
		break ;
	}
	if (target == -1)
		return ;
	string_free(&env->symbols[target].name);
	string_free(&env->symbols[target].value);
	bytes_move(&env->symbols[target], &env->symbols[target + 1], sizeof(t_symbol) * (env->symbols_size - target));
	env->symbols_size--;
}

void	env_put_var(t_environment *env, t_symbol symbol)
{
	size_t	index;

	index = -1;
	if (string_equals(symbol.name.value, "_"))
	{
		string_free(&symbol.name);
		string_free(&symbol.value);
		return ;
	}
	while (++index < env->symbols_size)
	{
		if (string_equals(env->symbols[index].name.value, symbol.name.value))
		{
			string_free(&env->symbols[index].name);
			string_free(&env->symbols[index].value);
			env->symbols[index].name = symbol.name;
			env->symbols[index].value = symbol.value;
			return ;
		}
	}
	env_insert_var(env, symbol);
}