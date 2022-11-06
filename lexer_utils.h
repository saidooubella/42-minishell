/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 10:50:56 by soubella          #+#    #+#             */
/*   Updated: 2022/11/04 10:51:42 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_UTILS_H
# define LEXER_UTILS_H

# include <stdbool.h>

bool	is_identifier_start(char c);
bool	is_identifier_cont(char c);
bool	is_space(char c);

#endif