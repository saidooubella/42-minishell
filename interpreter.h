/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:40:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/08 18:19:48 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERPRETER_H
# define INTERPRETER_H

# include <string.h>

# include "string_utils.h"
# include "utils.h"

typedef struct s_symbol
{
	char	*name;
	char	*value;
}	t_symbol;

typedef struct s_environment
{
	t_symbol	*symbols;
	size_t		symbols_size;
	size_t		symbols_cap;
}	t_environment;

void	env_insert_var(t_environment *env, t_symbol symbol);
void	env_put_var(t_environment *env, t_symbol symbol);
void	env_remove_var(t_environment *env, char *name);
char	*env_get_var(t_environment *env, char *name);

#endif
