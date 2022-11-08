/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:42:13 by soubella          #+#    #+#             */
/*   Updated: 2022/11/08 10:52:40 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_UTILS_H
# define STRING_UTILS_H

# include <stdbool.h>
# include <stddef.h>

char	*substring(char *string, size_t start, size_t end);
bool	string_contains(char const *s1, char ch);
char	*string_duplicate(char *s1);
size_t	string_length(const char *string);

#endif
