/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 14:06:27 by soubella          #+#    #+#             */
/*   Updated: 2022/11/12 17:12:04 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

int					g_in_fd;
struct sigaction	g_int_action;
struct sigaction	g_quit_action;

void	register_handler(struct sigaction *action, int signum, void f(int));
void	sigint_close_handler(int signum);
void	sigint_handler(int signum);

#endif
