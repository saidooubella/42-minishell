/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:40:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/15 11:26:31 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERPRETER_H
# define INTERPRETER_H

# include <string.h>

# include "string_utils.h"
# include "environment.h"
# include "string.h"
# include "utils.h"
# include "nodes.h"

# define BUILTINS_COUNT 7

typedef struct s_result
{
	bool	success;
	int		extra;
}	t_result;

t_result		visit_node(t_environment *env, t_node *node);

#endif
