/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin_1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 13:05:40 by soubella          #+#    #+#             */
/*   Updated: 2022/11/26 14:21:04 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>

#include "string_utils.h"
#include "environment.h"
#include "ft_printf.h"
#include "utils.h"

int	cd_access_parent_dir(t_environment *env);
int	cd_access_oldpwd(t_environment *env);
int	cd_access_home(t_environment *env);
int	cd_access_cwd(t_environment *env);

void	update_internals(t_environment *env, char *target, bool chcwd)
{
	t_symbol	symbol;

	symbol.name = string_create("OLDPWD", false);
	symbol.value = env->working_dir;
	env_put_var(env, symbol, true);
	if (chcwd)
	{
		if (chdir(target) == -1)
			error("Couldn't change the current working directory");
		env->working_dir = string_create(getcwd(NULL, 0), true);
	}
	else
	{
		env->working_dir = string_create(target, true);
	}
	symbol.name = string_create("PWD", false);
	symbol.value = string_create(env->working_dir.value, false);
	env_put_var(env, symbol, true);
}

char	*get_env_var(t_environment *env, char *name)
{
	char	*target;

	target = env_get_var(env, name, NULL);
	if (target == NULL)
	{
		ft_printf(STDERR_FILENO, "cd: %s not set\n", name);
		return (NULL);
	}
	if (access(target, F_OK) == -1)
	{
		ft_printf(STDERR_FILENO, "cd: %s: %s\n", target, strerror(errno));
		return (NULL);
	}
	return (target);
}

int	cd_builtin(t_environment *env, size_t argc, char **argv)
{
	char	*target;

	if (argc < 2)
		return (cd_access_home(env));
	else if (string_equals(argv[1], "-"))
		return (cd_access_oldpwd(env));
	else if (string_equals(argv[1], "."))
		return (cd_access_cwd(env));
	else if (string_equals(argv[1], ".."))
		return (cd_access_parent_dir(env));
	target = argv[1];
	if (access(target, F_OK) == -1)
	{
		ft_printf(STDERR_FILENO, "cd: %s: %s\n", target, strerror(errno));
		return (1);
	}
	update_internals(env, target, true);
	return (0);
}
