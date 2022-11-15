/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_resolver.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 16:31:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/14 17:30:30 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_REOLVER_H
# define EXEC_REOLVER_H

# include "interpreter.h"
# include "string.h"

typedef enum e_error_type
{
	NONE,
	NOT_FOUND,
	PERMISSION_DENIED,
}	t_error_type;

typedef struct s_resolve_result
{
	bool				success;
	t_error_type		err_type;
	t_optional_string	executable_path;
}	t_resolve_result;

char				*error_msg(t_error_type err_type);
t_resolve_result	resolve_executable(t_environment *env, char *executable);

#endif
