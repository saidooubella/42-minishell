/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 14:06:27 by soubella          #+#    #+#             */
/*   Updated: 2022/11/28 10:10:16 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include "environment.h"
# include "lexer.h"

typedef struct s_globals
{
	bool			handle_signals;
	bool			in_pipe_node;
	int				in_fd;
	t_environment	*env;
}	t_globals;

extern t_globals	g_globals;

int		entry_point(char **env, bool is_bonus);
void	initialization(char **env, bool is_bonus);
void	exec_command(t_lexer_result *result);
char	*read_command_line(void);

void	sigint_default_handler(int signum);
void	sigint_heredoc_handler(int signum);

#endif
