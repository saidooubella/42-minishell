/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 12:00:36 by soubella          #+#    #+#             */
/*   Updated: 2022/11/18 15:41:12 by soubella         ###   ########.fr       */
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
	int		(*block)(t_environment *env, size_t argc, t_elements **argv);
}	t_builtin;

int			echo_builtin(t_environment *env, size_t argc, t_elements **argv);
int			cd_builtin(t_environment *env, size_t argc, t_elements **argv);
int			pwd_builtin(t_environment *env, size_t argc, t_elements **argv);
int			export_builtin(t_environment *env, size_t argc, t_elements **argv);
int			unset_builtin(t_environment *env, size_t argc, t_elements **argv);
int			env_builtin(t_environment *env, size_t argc, t_elements **argv);
int			exit_builtin(t_environment *env, size_t argc, t_elements **argv);
t_builtin	*builtin_lookup(t_builtin *builtins, char *target);
t_builtin	*initilize_builtins(void);

#endif
