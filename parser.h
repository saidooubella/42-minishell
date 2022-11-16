/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 10:58:25 by soubella          #+#    #+#             */
/*   Updated: 2022/11/15 11:16:42 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "interpreter.h"
# include "tokens.h"
# include "string.h"
# include "nodes.h"

typedef struct s_parser
{
	t_environment	*env;
	t_tokens		*tokens;
	t_tokens		*leftovers;
	size_t			leftovers_head;
	size_t			index;
}	t_parser;

t_optional_node	parse(t_parser *parser);
t_parser		*parser_new(t_tokens *tokens, t_environment *env);
void			parser_free(t_parser **parser);

#endif
