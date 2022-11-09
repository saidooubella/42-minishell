/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 14:38:56 by soubella          #+#    #+#             */
/*   Updated: 2022/11/09 14:39:48 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_H
# define STRING_H

# include <stdbool.h>

typedef	struct s_string
{
	char	*value;
	bool	freeable;
}	t_string;

t_string	string_create(char *value, bool freeable);
void		string_free(t_string *string);

#endif