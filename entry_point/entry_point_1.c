/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entry_point_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 11:01:21 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 20:46:49 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#include "string_utils.h"
#include "environment.h"
#include "interpreter.h"
#include "entry_point.h"
#include "ft_printf.h"
#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "nodes.h"

t_globals	g_globals;

int	entry_point(char **env, bool is_bonus)
{
	t_lexer_result	result;
	t_lexer			*lexer;
	char			*line;

	initialization(env, is_bonus);
	while (true)
	{
		line = read_command_line();
		if (line == NULL)
			break ;
		lexer = lexer_new(g_globals.env, line);
		result = lexer_tokenize(lexer);
		if (!result.success)
			g_globals.env->exit_code = 258;
		exec_command(&result);
		tokens_free(&result.tokens);
		lexer_free(&lexer);
	}
	if (isatty(STDIN_FILENO))
		ft_printf(STDOUT_FILENO, "\033[1A\033[3Cexit\n");
	env_free(&g_globals.env);
	return (0);
}
