/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_matcher.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/13 16:07:50 by soubella          #+#    #+#             */
/*   Updated: 2022/11/13 16:08:08 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WILDCARD_MATCHER_H
# define WILDCARD_MATCHER_H

# include "string.h"

t_string apply_pattern(char *pattern, char *directory);

#endif
