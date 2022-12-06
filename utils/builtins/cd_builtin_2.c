/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 13:06:27 by soubella          #+#    #+#             */
/*   Updated: 2022/11/26 13:43:35 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>

#include "path_simplifier.h"
#include "string_utils.h"
#include "environment.h"
#include "ft_printf.h"

void	update_internals(t_environment *env, char *target, bool chcwd);
char	*get_env_var(t_environment *env, char *name);

int	cd_access_oldpwd(t_environment *env)
{
	char	*target;

	target = get_env_var(env, "OLDPWD");
	if (target == NULL)
		return (1);
	ft_printf(STDERR_FILENO, "%s\n", target);
	update_internals(env, target, true);
	return (0);
}

int	cd_access_cwd(t_environment *env)
{
	char	*target;

	target = getcwd(NULL, 0);
	if (target == NULL)
	{
		ft_printf(STDERR_FILENO, "cd: .: No such file or directory\n");
		free(target);
		target = string_join(env->working_dir.value, "/.");
		update_internals(env, target, false);
		return (1);
	}
	update_internals(env, target, true);
	free(target);
	return (0);
}

int	cd_access_parent_dir(t_environment *env)
{
	char	*_target;
	char	*target;

	_target = string_join(env->working_dir.value, "/..");
	target = simplify_path(_target);
	if (access(target, F_OK) == -1)
	{
		ft_printf(STDERR_FILENO, "cd: ..: No such file or directory\n");
		free(target);
		update_internals(env, _target, false);
		return (1);
	}
	free(_target);
	update_internals(env, target, true);
	free(target);
	return (0);
}

int	cd_access_home(t_environment *env)
{
	char	*target;

	target = get_env_var(env, "HOME");
	if (target == NULL)
		return (1);
	update_internals(env, target, true);
	return (0);
}
