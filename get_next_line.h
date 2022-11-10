/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/07 14:51:26 by soubella          #+#    #+#             */
/*   Updated: 2022/11/10 14:50:12 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GET_NEXT_LINE_H
# define FT_GET_NEXT_LINE_H

# include <sys/types.h>

# define DEFAULT_CAPACITY 16
# define BUFFER_SIZE 8192

typedef struct s_state_holder
{
	char	buffer[BUFFER_SIZE];
	ssize_t	read_size;
	size_t	offset;
}	t_state_holder;

typedef struct s_read_info
{
	int		has_newline;
	size_t	line_size;
}	t_read_info;

char				*get_next_line(int fd);

#endif
