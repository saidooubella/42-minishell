/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:42:13 by soubella          #+#    #+#             */
/*   Updated: 2022/11/14 11:25:11 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_UTILS_H
# define STRING_UTILS_H

# include <sys/types.h>
# include <stdbool.h>
# include <stddef.h>

char	**string_split(char *string, char *seps);
char	*substring(char *string, size_t start, size_t end);
bool	string_contains(char const *s1, char ch);
char	*string_duplicate(char *s1);
size_t	string_length(const char *string);
int		string_compare(const char *s1, const char *s2, size_t n);
int		string_equals(char *s1, char *s2);
char	*string_join(char const *s1, char const *s2);
int		are_equals(char *s1, char *s2);
char	*int_to_string(int number);
bool	string_starts_with(char *str, char *to_find, size_t to_find_len);
ssize_t	string_index_of(char *target, char *str);
int		string_to_int(char *str);

#endif
