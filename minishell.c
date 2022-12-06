/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 10:10:43 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 17:00:29 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "entry_point.h"

int	main(int ac, char **av, char **env)
{
	(void) ac;
	(void) av;
	return (entry_point(env, false));
}
