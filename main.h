/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 14:06:27 by soubella          #+#    #+#             */
/*   Updated: 2022/11/15 13:47:10 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include "environment.h"

typedef struct s_globals
{
	int				in_fd;
	bool			handle_signals;
	t_environment	*env;
}	t_globals;

t_globals	g_globals;

void	sigint_close_handler(int signum);
void	sigint_handler(int signum);

#endif
