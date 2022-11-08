/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 11:01:21 by soubella          #+#    #+#             */
/*   Updated: 2022/11/08 18:54:19 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "string_utils.h"
#include "interpreter.h"
#include "parser.h"
#include "lexer.h"
#include "utils.h"

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
	if (node->type == COMMAND_NODE)
		command_node_free((t_command_node *) node);
	else if (node->type == PARENTHESES_NODE)
		parent_node_free((t_parent_node *) node);
	else if (node->type == PIPELINE_NODE)
		pipe_node_free((t_pipe_node *) node);
	else if (node->type == CONJUCTION_NODE)
		conjuction_node_free((t_conjuction_node *) node);
	else
		error("Illegal state in 'node_free'");
}

void	check_leaks(void)
{
	system("leaks minishell");
}

int	main(int ac, char **av, char **env)
{
	t_tokens	*tokens;
	t_lexer		*lexer;
	char		*line;

	atexit(check_leaks); // TODO - Remove this

	t_environment environment;
	environment.symbols = NULL;
	environment.symbols_size = 0;
	environment.symbols_cap = 0;

	while (*env)
	{
		char **splitted = string_split(*env++, '=');
		t_symbol symbol;
		symbol.name = splitted[0];
		symbol.value = splitted[1];
		env_put_var(&environment, symbol);
		free(splitted);
	}

	(void) ac;
	(void) av;
	while (true)
	{
		line = readline("$> ");
		if (line == NULL)
			break ;
		lexer = lexer_new(line);
		tokens = lexer_tokenize(lexer);
		add_history(line);
		if (tokens->size > 1)
		{
			// TODO
			for (size_t i = 0; i < tokens->size; i++) {
				printf("%s|%d\n", tokens->tokens[i].lexeme, tokens->tokens[i].type);
			}
			t_parser	parser;
			parser.index = 0;
			parser.tokens = tokens;
			node_free(parse(&parser));
		}
		tokens_free(&tokens);
		lexer_free(&lexer);
	}
}

/*

	// NOTE - ARGS ARE CASE INSENSETIVE

	// TODO   - U HAVE TWO MEMCPY AND SOME PRINTF CALLS IN UR CODE U GOTTA REMOVE 'EM

	&& - execute right when left successed.
	|| - execute right when left fail.
	|  - redirect the out of left to the in of the right.
	$? - works only with forground pipelines.

	operators : '&&' '||' '|' '<' '>' '>>' '<<' 

	// ANCHOR - Grammar
	
	expresions	: 	and_or+

	and_or		:	pipeline ('&&' | '||') pipeline

	pipeline	:	prefix '|' prefix

	prefix		:	((('<' | '>' | '>>') FILE_NAME) | ('<<' DELIMITER)) prefix
				|	suffix

	suffix		:	atom+ ((('<' | '>' | '>>') FILE_NAME) | ('<<' DELIMITER))

	atom		:	WORD
				|	'"([^"]|$[a-z_][a-z0-9_]+)*"'
				|	"'([^'])*'"
				|	'(' start ')'

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
