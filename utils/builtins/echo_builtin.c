/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:16:05 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 17:44:33 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#include "environment.h"
#include "string_utils.h"
#include "ft_printf.h"

static bool	parse_echo_flag(size_t argc, size_t *start, char **argv)
{
	bool	new_line;
	bool	loop;
	size_t	index;
	char	*arg;

	new_line = true;
	loop = true;
	while (loop && ++(*start) < argc)
	{
		arg = argv[*start];
		if (!string_starts_with(arg, "-", 1) || string_equals(arg, "-"))
			break ;
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
	return (new_line);
}

int	echo_builtin(t_environment *env, size_t argc, char **argv)
{
	bool	new_line;
	size_t	start;
	size_t	cont;

	(void) env;
	start = 0;
	new_line = parse_echo_flag(argc, &start, argv);
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
