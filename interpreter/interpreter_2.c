/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:30:00 by soubella          #+#    #+#             */
/*   Updated: 2022/12/03 15:33:29 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <signal.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "environment.h"
#include "ft_printf.h"
#include "parser.h"

t_result	result_create(t_result_type type, int extra)
{
	t_result	result;

	result.extra = extra;
	result.type = type;
	return (result);
}

static size_t	calc_args_size(
	t_environment *env, t_command_node *node, bool unwrap_all)
{
	t_string	resolved;
	t_elements	*temp;
	size_t		index;
	size_t		size;

	index = -1;
	size = 0;
	while (++index < node->args_size)
	{
		temp = node->args[index];
		if (is_empty_argument(temp, env))
			continue ;
		if (unwrap_all)
		{
			resolved = elements_resolve(temp, env);
			if (string_length(resolved.value) != 0)
				size += calc_words_count(resolved.value, " \t\f\n\r\v");
			else
				size += 1;
			string_free(&resolved);
		}
		else
			size += 1;
	}
	return (size);
}

static char	**resolve_arg(
	t_environment *env, t_elements *target,
	char **mapped, bool unwrap_all)
{
	t_string	resolved;
	char		**splitted;
	char		**temp;

	if (is_empty_argument(target, env))
		return (mapped);
	resolved = elements_resolve(target, env);
	if (unwrap_all)
	{
		splitted = string_split(resolved.value, " \t\f\n\r\v");
		string_free(&resolved);
		if (*splitted == NULL)
			*mapped++ = string_duplicate("");
		else
		{
			temp = splitted;
			while (*temp)
				*mapped++ = *temp++;
		}
		free(splitted);
	}
	else
		*mapped++ = resolved.value;
	return (mapped);
}

char	**unwrap_args(
	t_environment *env, t_command_node *node,
	size_t *actual_size, bool unwrap_all)
{
	size_t	args_index;
	size_t	index;
	char	**mapped;
	char	**temp;

	args_index = -1;
	*actual_size = calc_args_size(env, node, unwrap_all);
	mapped = malloc(sizeof(char *) * (*actual_size + 1));
	if (mapped == NULL)
		memory_error();
	args_index = -1;
	index = 0;
	while (++args_index < node->args_size)
	{
		temp = resolve_arg(env, node->args[args_index],
				mapped + index, unwrap_all);
		index += temp - (mapped + index);
	}
	mapped[index] = NULL;
	return (mapped);
}

char	**unwrap_env(t_symbol *syms, size_t size)
{
	size_t	index;
	char	**mapped;
	char	*str1;
	char	*str2;

	mapped = malloc(sizeof(char *) * (size + 1));
	if (mapped == NULL)
		memory_error();
	index = -1;
	while (++index < size)
	{
		str1 = string_join(syms[index].name.value, "=");
		str2 = string_join(str1, syms[index].value.value);
		free(str1);
		mapped[index] = str2;
	}
	mapped[index] = NULL;
	return (mapped);
}
