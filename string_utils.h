/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:42:13 by soubella          #+#    #+#             */
/*   Updated: 2022/12/02 13:56:37 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_UTILS_H
# define STRING_UTILS_H

# include <sys/types.h>
# include <stdbool.h>
# include <stddef.h>

# define _LONG_MAX 9223372036854775807ull
# define _LONG_MIN 9223372036854775808ull

bool		string_starts_with(char *str, char *to_find, size_t to_find_len);
char		*string_trim(char *str, bool keep_left_one, bool keep_right_one);
int			string_compare(const char *s1, const char *s2, size_t n);
long long	string_to_llong(char *str, bool allow_neg, bool *error);
void		*bytes_move(void *dst, const void *src, size_t len);
char		*substring(char *string, size_t start, size_t end);
void		*bytes_copy(void *dst, const void *src, size_t n);
bool		string_contains_any(char const *s1, char *chrs);
char		*string_join(char const *s1, char const *s2);
size_t		calc_words_count(char *string, char *seps);
bool		string_contains(char const *s1, char ch);
ssize_t		string_index_of(char *target, char *str);
char		**string_split(char *string, char *seps);
char		*llong_to_string(long long number);
size_t		string_length(const char *string);
int			string_equals(char *s1, char *s2);
bool		string_is_blank(char const *s1);
int			are_equals(char *s1, char *s2);
char		*string_duplicate(char *s1);
bool		is_whitespace(char c);

#endif
