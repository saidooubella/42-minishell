/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:20:00 by soubella          #+#    #+#             */
/*   Updated: 2022/11/09 15:13:55 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>

#include "interpreter.h"
#include "parser.h"
#include "utils.h"



t_environment *environment_new(char **env)
{
	t_environment	*environment;

	environment = malloc(sizeof(t_environment));
	if (environment == NULL)
		memory_error();
	environment->symbols = NULL;
	environment->last_exit_code = 0;
	environment->symbols_size = 0;
	environment->symbols_cap = 0;
	environment->running = true;
	environment->working_dir = string_create(getcwd(NULL, 0), true);
	if (environment->working_dir.value == NULL)
		error("Error: Cannot retreive the current working directory");
	while (*env)
	{
		char **splitted = string_split(*env++, '=');
		t_symbol symbol;
		symbol.name = string_create(splitted[0], splitted[0] != NULL);
		symbol.value = string_create(splitted[1], splitted[1] != NULL);
		env_put_var(environment, symbol);
		free(splitted);
	}
	return (environment);
}

void	environment_free(t_environment **env)
{
	size_t	index;

	index = -1;
	while (++index < (*env)->symbols_size)
	{
		string_free(&(*env)->symbols[index].name);
		string_free(&(*env)->symbols[index].value);
	}
	string_free(&(*env)->working_dir);
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
		memcpy(new_symbols, env->symbols, env->symbols_size * sizeof(t_symbol));
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
	while (++index < env->symbols_size)
	{
		if (string_equals(env->symbols[index].name.value, name))
		{
			target = index;
			break ;
		}
	}
	if (target == -1)
		return ;
	memmove(&env->symbols[target], &env->symbols[target + 1], sizeof(t_symbol) * (env->symbols_size - target));
	env->symbols_size--;
}

void	env_put_var(t_environment *env, t_symbol symbol)
{
	size_t	index;

	index = -1;
	while (++index < env->symbols_size)
	{
		if (string_equals(env->symbols[index].name.value, symbol.name.value))
		{
			string_free(&env->symbols[index].value);
			env->symbols[index].value = symbol.value;
			return ;
		}
	}
	env_insert_var(env, symbol);
}
