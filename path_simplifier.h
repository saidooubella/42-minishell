/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_simplifier.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/20 16:02:15 by soubella          #+#    #+#             */
/*   Updated: 2022/11/20 16:04:44 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_SIMPLIFIER_H
# define PATH_SIMPLIFIER_H

typedef struct s_path
{
	char 			*part;
	struct s_path	*next;
}	t_path;

char	*simplify_path(char *path);

#endif
