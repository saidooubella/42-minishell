/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 12:00:36 by soubella          #+#    #+#             */
/*   Updated: 2022/11/10 13:55:32 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

int	echo_builtin(t_environment *env, size_t argc, t_string *argv);
int	cd_builtin(t_environment *env, size_t argc, t_string *argv);
int	pwd_builtin(t_environment *env, size_t argc, t_string *argv);
int	export_builtin(t_environment *env, size_t argc, t_string *argv);
int	unset_builtin(t_environment *env, size_t argc, t_string *argv);
int	env_builtin(t_environment *env, size_t argc, t_string *argv);
int	exit_builtin(t_environment *env, size_t argc, t_string *argv);

#endif
