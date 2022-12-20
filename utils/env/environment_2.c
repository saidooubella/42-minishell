/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:27:58 by soubella          #+#    #+#             */
/*   Updated: 2022/12/20 21:51:17 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>

#include "string_utils.h"
#include "environment.h"
#include "ft_printf.h"

char	*env_get_var(t_environment *env, char *name, char *fallback)
{
	size_t	index;
	char	*temp;

	index = -1;
	while (++index < env->symbols_size)
	{
		if (string_equals(env->symbols[index].name.value, name))
		{
			temp = env->symbols[index].value.value;
			if (temp == NULL)
				temp = "";
			return (temp);
		}
	}
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
	bytes_move(&env->symbols[target], &env->symbols[target + 1],
		sizeof(t_symbol) * (env->symbols_size - target));
	env->symbols_size--;
}

void	env_put_var(
	t_environment *env, t_symbol symbol, bool force, bool has_value)
{
	size_t	index;

	index = -1;
	symbol.has_value = has_value;
	if (!force && string_equals(symbol.name.value, "_"))
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
			env->symbols[index].has_value = symbol.has_value;
			return ;
		}
	}
	env_insert_var(env, symbol);
}
