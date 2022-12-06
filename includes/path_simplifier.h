/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_simplifier.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/20 16:02:15 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:11:22 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_SIMPLIFIER_H
# define PATH_SIMPLIFIER_H

# include "sys/types.h"

typedef struct s_path
{
	char			*part;
	struct s_path	*next;
}	t_path;

typedef struct s_pfs_locals
{
	size_t	path_len;
	size_t	offset;
	t_path	*head;
	t_path	*tail;
	ssize_t	index;
	t_path	**target;
	char	*sub;
}	t_pfs_locals;

void	path_free(t_path *path);
t_path	*path_new(char *part);
t_path	*path_from_string(char *path);
char	*simplify_path(char *path);

#endif
