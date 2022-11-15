/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 11:01:21 by soubella          #+#    #+#             */
/*   Updated: 2022/11/14 18:14:17 by soubella         ###   ########.fr       */
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
#include "interpreter.h"
#include "ft_printf.h"
#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "nodes.h"
#include "main.h"

void	node_free(t_node *node);

void	command_node_free(t_command_node *node)
{
	size_t	index;

	index = -1;
	while (++index < node->redirections_size)
		string_free(&node->redirections[index].extra);
	index = -1;
	while (++index < node->args_size)
		string_free(&node->args[index]);
	free(node->redirections);
	free(node->args);
	free(node);
}

void	parent_node_free(t_parent_node *node)
{
	size_t	index;

	index = -1;
	node_free(node->expression);
	free(node);
}

void	pipe_node_free(t_pipe_node *node)
{
	size_t	index;

	index = -1;
	node_free(node->right);
	node_free(node->left);
	free(node);
}

void	conjuction_node_free(t_conjuction_node *node)
{
	size_t	index;

	index = -1;
	node_free(node->right);
	node_free(node->left);
	free(node);
}

void	node_free(t_node *node)
{
	if (node == NULL)
		return ;
	if (node->type == COMMAND_NODE)
		command_node_free((t_command_node *) node);
	else if (node->type == PARENTHESES_NODE)
		parent_node_free((t_parent_node *) node);
	else if (node->type == PIPELINE_NODE)
		pipe_node_free((t_pipe_node *) node);
	else if (node->type == CONJUCTION_NODE)
		conjuction_node_free((t_conjuction_node *) node);
	else
		ft_printf(STDERR_FILENO, "Error: Illegal state in 'node_free'\n");
}

void	check_leaks(void)
{
	system("leaks minishell");
}

void	sigint_handler(int signum)
{
	if (signum != SIGINT)
		return ;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	sigint_close_handler(int signum)
{
	if (signum != SIGINT)
		return ;
	g_in_fd = dup(STDOUT_FILENO);
	close(STDIN_FILENO);
}

void	register_handler(struct sigaction *action, int signum, void f(int))
{
	action->sa_handler = f;
	action->sa_flags = 0;
	action->sa_mask = 0;
	sigaction(signum, action, NULL);
}

int	main(int ac, char **av, char **env)
{
	t_lexer_result	result;
	t_lexer			*lexer;
	char			*line;

	(void) ac;
	(void) av;
	t_environment *environment = environment_new(env);

	register_handler(&g_quit_action, SIGQUIT, SIG_IGN);

	rl_catch_signals = 0;

	while (environment->running)
	{
		register_handler(&g_int_action, SIGINT, sigint_handler);
		if (isatty(STDIN_FILENO))
			line = readline("$> ");
		else
			line = readline(NULL);
		if (line == NULL)
			break ;
		register_handler(&g_int_action, SIGINT, SIG_IGN);
		add_history(line);
		lexer = lexer_new(line);
		result = lexer_tokenize(lexer);
		if (!result.success)
			environment->last_exit_code = 258;
		if (result.success && result.tokens->size > 1)
		{
			// NOTE - Remove it
			// for (size_t i = 0; i < result.tokens->size; i++) {
			// 	printf("%s|%d\n", result.tokens->tokens[i].lexeme, result.tokens->tokens[i].type);
			// }
			t_parser	parser;
			parser.index = 0;
			parser.tokens = result.tokens;
			parser.env = environment;
			parser.leftovers = tokens_new();
			parser.leftovers_head = 0;
			t_optional_node root = parse(&parser);
			if (root.present)
			{
				environment->last_exit_code = visit_node(environment, root.node, -1, -1, -1, true).extra;
			}
			else
				environment->last_exit_code = 258;
			node_free(root.node);
			tokens_free(&parser.leftovers);
		}
		tokens_free(&result.tokens);
		lexer_free(&lexer);
	}
	if (isatty(STDIN_FILENO))
		ft_printf(STDOUT_FILENO, "exit\n");
	environment_free(&environment);
	check_leaks();
}

/*

	SIGINT  - Ctrl+C
	SIGQUIT - Ctrl+\

*/

/*

	// NOTE - ARGS ARE CASE INSENSETIVE

	// TODO - U HAVE SOME MEMCPY AND PRINTF CALLS IN UR CODE U GOTTA REMOVE 'EM

	&& - execute right when left successed.
	|| - execute right when left fail.
	|  - redirect the out of left to the in of the right.
	$? - works only with forground pipelines.

	operators : '&&' '||' '|' '<' '>' '>>' '<<' 

	// ANCHOR - Grammar
	
	expresions	: 	and_or+

	and_or		:	pipeline ('&&' | '||') pipeline

	pipeline	:	prefix '|' prefix

	atom		:	WORD
				|	'"([^"]|$[a-z_][a-z0-9_]+)*"'
				|	"'([^'])*'"
				|	'(' expresions ')'
				|	'<<'	LIMITER
				|	'>>'	FILE
				|	'>'		FILE
				|	'<'		FILE

*/

/*
	- does '||' and '&&' have the same precedence? -- yes, they sure do.
	- when a match failed should I just fallback to the match string or should I report an error? -- fallback
	- is '< infile command' equivalent to 'command < infile'? -- yes
	- should I handle '$()' and '${}'? -- nope
	- should I handle '/ *' or '*.c'? -- yes
	- should I ignore whitespace? -- yes
	- should I handle escaping? -- yes
*/

/*
 - Requirements:
    - A prompt.
	- History.
	- Executing the right executables (using the PATH env. var. using both relative and absolute paths).
	- DONT INTERPRET UNCLOSED QUOTES, BACKSLASH '\', SEMICOLONS ';' OR ANY OTHER SPECIAL CHARACTER.
	- Handle single quote and double quotes.
	- Implements redirections '<' and '>'.
	- Implement heredoc without the need to update the history.
	- Implement output redirection with the append mode '>>'.
	- Implement pipes '|'.
	- Handle env. vars. '$name'.
	- Handle '$?' (the exit code of the recently executed foreground pipeline).
	- Handle 'Ctrl+C' (displays a new prompt on a new line), 'Ctrl+D' (exits the shell) and 'Ctrl+/' (does nothing).
	- Implement 'echo' with the flag '-n', 'cd', 'pwd', 'export', 'unset', 'env' and 'exit'.
	- Implement '&&' and '||' with respect to the precedence.
	- The wildcard * must be handled for the current working directory.
*/
