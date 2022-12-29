/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:22:00 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 17:59:32 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include "string_utils.h"
#include "environment.h"
#include "ft_printf.h"

static int	exit_cont(size_t argc, char	*arg, int64_t exit_code, bool error)
{
	if (error)
	{
		exit_code = 255;
		ft_printf(STDERR_FILENO, "exit: %s: numeric argument required\n", arg);
	}
	else if (argc > 2)
	{
		ft_printf(STDERR_FILENO, "exit: too many arguments\n");
		return (1);
	}
	exit(exit_code & 0xFF);
	return (0);
}

int	exit_builtin(t_environment *env, size_t argc, char **argv)
{
	int64_t	exit_code;
	bool	error;
	char	*arg;

	(void) env;
	if (argc <= 1)
	{
		ft_printf(STDOUT_FILENO, "exit\n");
		exit(0);
	}
	arg = argv[1];
	exit_code = string_to_llong(arg, true, &error);
	ft_printf(STDOUT_FILENO, "exit\n", arg);
	return (exit_cont(argc, arg, exit_code, error));
}
