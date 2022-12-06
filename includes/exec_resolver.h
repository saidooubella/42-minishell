/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_resolver.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 16:31:35 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 15:37:25 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_RESOLVER_H
# define EXEC_RESOLVER_H

# include "free_aware_string.h"
# include "interpreter.h"

char	*resolve_executable(t_environment *env, char *executable);

#endif
