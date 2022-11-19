/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 11:21:46 by soubella          #+#    #+#             */
/*   Updated: 2022/11/17 20:54:52 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENVIRONEMNT_H
# define ENVIRONEMNT_H

# include <sys/types.h>

# include "string.h"
# include "builtins.h"

typedef struct s_symbol
{
	t_string	name;
	t_string	value;
}	t_symbol;

typedef struct s_environment
{
	t_symbol	*symbols;
	t_string	working_dir;
	size_t		symbols_size;
	size_t		symbols_cap;
	int			exit_code;
	t_builtin	*builtins;
}	t_environment;

char			*env_get_var(t_environment *env, char *name, char *fallback);
void			env_insert_var(t_environment *env, t_symbol symbol);
void			env_put_var(t_environment *env, t_symbol symbol, bool force);
void			env_remove_var(t_environment *env, char *name);
void			env_free(t_environment **env);
t_environment	*env_new(char **env);

#endif
