/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:40:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/15 09:51:40 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERPRETER_H
# define INTERPRETER_H

# include <string.h>

# include "string_utils.h"
# include "string.h"
# include "utils.h"
# include "nodes.h"

# define BUILTINS_COUNT 7

typedef struct s_symbol
{
	t_string	name;
	t_string	value;
}	t_symbol;

typedef struct s_builtin	t_builtin;

typedef struct s_environment
{
	t_symbol	*symbols;
	t_string	working_dir;
	size_t		symbols_size;
	size_t		symbols_cap;
	int			last_exit_code;
	bool		running;
	t_builtin	*builtins;
}	t_environment;

typedef struct s_builtin
{
	char	*name;
	int		(*block)(t_environment *env, size_t argc, t_string *argv);
}	t_builtin;

typedef struct s_result
{
	bool	success;
	int		extra;
}	t_result;

t_result		visit_node(t_environment *env, t_node *node, int in, int out, int to_be_closed, bool should_wait);
char			*env_get_var(t_environment *env, char *name, char *fallback);
void			env_insert_var(t_environment *env, t_symbol symbol);
void			env_put_var(t_environment *env, t_symbol symbol);
void			env_remove_var(t_environment *env, char *name);
void			environment_free(t_environment **env);
t_environment	*environment_new(char **env);
t_builtin		*initilize_builtins(void);

#endif
