/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:40:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/17 15:37:03 by soubella         ###   ########.fr       */
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

typedef enum e_result_type
{
	EXIT_STATUS,
	ERROR,
	PID,
}	t_result_type;

typedef struct s_result
{
	t_result_type	type;
	int				extra;
}	t_result;

t_result		visit_node(t_environment *env, t_node *node);

#endif
