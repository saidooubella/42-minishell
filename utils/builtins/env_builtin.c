/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:21:10 by soubella          #+#    #+#             */
/*   Updated: 2022/11/28 17:54:45 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <unistd.h>

#include "environment.h"
#include "ft_printf.h"

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
		if (!symbol.has_value)
			continue ;
		ft_printf(STDOUT_FILENO, "%s=", symbol.name.value);
		if (symbol.value.value != NULL)
			ft_printf(STDOUT_FILENO, "%s", symbol.value.value);
		ft_printf(STDOUT_FILENO, "\n");
	}
	return (0);
}
