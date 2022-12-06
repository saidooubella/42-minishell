/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 10:37:37 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 17:53:58 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>

#include "environment.h"
#include "interpreter.h"
#include "element.h"

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

t_builtin	*builtin_lookup(t_environment *env, t_elements *target)
{
	size_t		index;
	t_string	path;

	path = elements_resolve(target, env);
	index = -1;
	while (++index < BUILTINS_COUNT)
	{
		if (string_equals(env->builtins[index].name, path.value))
		{
			string_free(&path);
			return (&env->builtins[index]);
		}
	}
	string_free(&path);
	return (NULL);
}
