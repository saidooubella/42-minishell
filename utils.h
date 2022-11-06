/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 13:45:27 by soubella          #+#    #+#             */
/*   Updated: 2022/11/06 13:29:44 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "lexer.h"

void	tokens_smart_add(
			t_lexer *lexer, t_tokens *tokens,
			char *lexeme, t_token_type type);
void	error(char *fmt, ...);
void	memory_error(void);

#endif
