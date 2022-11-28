/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:13:30 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 18:24:11 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "parser.h"

t_parser	*parser_new(t_tokens *tokens, t_environment *env)
{
	t_parser	*parser;

	parser = malloc(sizeof(t_parser));
	if (parser == NULL)
		memory_error();
	parser->index = 0;
	parser->tokens = tokens;
	parser->env = env;
	return (parser);
}

void	parser_free(t_parser **parser)
{
	free(*parser);
}

bool	parser_reached_end(t_parser *parser)
{
	return (parser->tokens->tokens[parser->index].type == END_OF_FILE);
}

bool	parser_current_is(t_parser *parser, t_token_type type)
{
	return (!parser_reached_end(parser)
		&& parser->tokens->tokens[parser->index].type == type);
}

t_token	*parser_consume(t_parser *parser)
{
	return (&parser->tokens->tokens[parser->index++]);
}
