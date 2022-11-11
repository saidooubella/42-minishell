/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 14:38:56 by soubella          #+#    #+#             */
/*   Updated: 2022/11/11 09:32:09 by soubella         ###   ########.fr       */
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

typedef struct s_optional_string
{
	t_string	string;
	bool		present;
}	t_optional_string;

t_optional_string	string_optional(t_string string, bool present);
t_string			string_create(char *value, bool freeable);
void				string_free(t_string *string);

#endif
