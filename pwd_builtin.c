/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:17:29 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 17:51:06 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stddef.h>

#include "environment.h"
#include "ft_printf.h"

int	pwd_builtin(t_environment *env, size_t argc, char **argv)
{
	(void) argc;
	(void) argv;
	ft_printf(STDOUT_FILENO, "%s\n", env->working_dir.value);
	return (0);
}
