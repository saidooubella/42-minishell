/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 16:10:46 by soubella          #+#    #+#             */
/*   Updated: 2022/11/28 10:07:38 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <signal.h>

#include "ft_printf.h"
#include "parser.h"
#include "lexer.h"
#include "main.h"

char	*read_command_line(void)
{
	char	*line;

	g_globals.handle_signals = true;
	if (isatty(STDIN_FILENO))
		line = readline("$> ");
	else
		line = readline(NULL);
	g_globals.handle_signals = false;
	if (line == NULL)
		return (NULL);
	add_history(line);
	return (line);
}

void	exec_command(t_lexer_result *result)
{
	t_parser		*parser;
	t_optional_node	root;

	if (!result->success || result->tokens->size <= 1)
		return ;
	if (!isatty(STDIN_FILENO))
		ft_printf(STDOUT_FILENO, "\033[1A\033[K");
	parser = parser_new(result->tokens, g_globals.env);
	root = parse(parser);
	if (root.present)
		g_globals.env->exit_code = visit_node(g_globals.env, root.node).extra;
	else
		g_globals.env->exit_code = 258;
	node_free(root.node);
	parser_free(&parser);
}

void	sigint_default_handler(int signum)
{
	if (signum != SIGINT || !g_globals.handle_signals)
		return ;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_globals.env->exit_code = 1;
}

void	sigint_heredoc_handler(int signum)
{
	if (signum != SIGINT)
		return ;
	g_globals.in_fd = dup(STDOUT_FILENO);
	close(STDIN_FILENO);
}

void	initialization(char **env, bool is_bonus)
{
	if (signal(SIGINT, sigint_default_handler) == SIG_ERR)
		error("Error: Failed to register a signal");
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		error("Error: Failed to register a signal");
	g_globals.handle_signals = true;
	g_globals.env = env_new(env, is_bonus);
	rl_catch_signals = 0;
}
