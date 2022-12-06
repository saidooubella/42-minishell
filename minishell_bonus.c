/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 10:10:59 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 15:11:25 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "entry_point.h"

int	main(int ac, char **av, char **env)
{
	(void) ac;
	(void) av;
	return (entry_point(env, true));
}
