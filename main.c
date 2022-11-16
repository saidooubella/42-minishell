/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 11:01:21 by soubella          #+#    #+#             */
/*   Updated: 2022/11/16 13:02:59 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#include <curses.h>
#include <term.h>

#include "string_utils.h"
#include "environment.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "nodes.h"
#include "main.h"

void	check_leaks(void)
{
	system("leaks minishell");
}

void	sigint_handler(int signum)
{
	if (signum != SIGINT || !g_globals.handle_signals)
		return ;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_globals.env->exit_code = 1;
}

void	sigint_close_handler(int signum)
{
	if (signum != SIGINT)
		return ;
	g_globals.in_fd = dup(STDOUT_FILENO);
	close(STDIN_FILENO);
}

void	register_siganals(void)
{
	if (signal(SIGINT, sigint_handler) == SIG_ERR ||
		signal(SIGQUIT, SIG_IGN) == SIG_ERR ||
		signal(SIGTSTP, SIG_IGN) == SIG_ERR)
		error("Error: Failed to register a signal");
}

int	main(int ac, char **av, char **env)
{
	t_lexer_result	result;
	t_lexer			*lexer;
	char			*line;

	(void) ac;
	(void) av;
	g_globals.env = env_new(env);
	register_siganals();
	rl_catch_signals = 0;
	while (true)
	{
		g_globals.handle_signals = true;
		if (isatty(STDIN_FILENO))
			line = readline("$> ");
		else
			line = readline(NULL);
		if (line == NULL)
			break ;
		g_globals.handle_signals = false;
		lexer = lexer_new(line);
		result = lexer_tokenize(lexer);
		if (!result.success)
			g_globals.env->exit_code = 258;
		if (result.success && result.tokens->size > 1)
		{
			if (!isatty(STDIN_FILENO))
				ft_printf(STDOUT_FILENO, "\033[1A\033[K");
			add_history(line);
			t_parser	*parser = parser_new(result.tokens, g_globals.env);
			t_optional_node root = parse(parser);
			if (root.present)
				g_globals.env->exit_code = visit_node(g_globals.env, root.node).extra;
			else
				g_globals.env->exit_code = 258;
			node_free(root.node);
			parser_free(&parser);
		}
		tokens_free(&result.tokens);
		lexer_free(&lexer);
	}
	if (isatty(STDIN_FILENO))
		ft_printf(STDOUT_FILENO, "\033[1A\033[3Cexit\n");
	env_free(&g_globals.env);
	check_leaks();
	return (0);
}
