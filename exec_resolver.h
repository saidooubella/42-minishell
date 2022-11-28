/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_resolver.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 16:31:35 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 14:30:41 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_RESOLVER_H
# define EXEC_RESOLVER_H

# include "interpreter.h"
# include "string.h"

char	*resolve_executable(t_environment *env, char *executable);

#endif
