/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_matcher.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/13 16:07:50 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 15:46:40 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WILDCARD_MATCHER_H
# define WILDCARD_MATCHER_H

# include "free_aware_string.h"

typedef struct s_matcher_locals
{
	size_t	pattern_len;
	size_t	target_len;
	size_t	str_index;
	size_t	text_len;
	size_t	index;
	char	*substr;
	char	*temp;
}	t_matcher_locals;

bool		wildcard_case(
				char *pattern, char *target, t_matcher_locals *locals);
bool		text_case(char *pattern, char *target, t_matcher_locals *locals);
t_string	apply_pattern(char *pattern, char *directory);

#endif
