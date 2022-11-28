/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 12:00:36 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 15:03:45 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include <sys/types.h>

# include "environment.h"
# include "element.h"
# include "string.h"

typedef struct s_environment	t_environment;

typedef struct s_builtin
{
	char	*name;
	int		(*block)(t_environment *env, size_t argc, char **argv);
}	t_builtin;

int			echo_builtin(t_environment *env, size_t argc, char **argv);
int			cd_builtin(t_environment *env, size_t argc, char **argv);
int			pwd_builtin(t_environment *env, size_t argc, char **argv);
int			export_builtin(t_environment *env, size_t argc, char **argv);
int			unset_builtin(t_environment *env, size_t argc, char **argv);
int			env_builtin(t_environment *env, size_t argc, char **argv);
int			exit_builtin(t_environment *env, size_t argc, char **argv);
t_builtin	*builtin_lookup(t_environment *env, t_elements *target);
t_builtin	*initilize_builtins(void);

#endif
