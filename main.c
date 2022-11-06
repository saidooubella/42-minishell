/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 11:01:21 by soubella          #+#    #+#             */
/*   Updated: 2022/11/06 17:58:39 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "parser.h"
#include "lexer.h"

void	check_leaks(void)
{
	system("leaks minishell");
}

int	main(void)
{
	t_tokens	*tokens;
	t_lexer		*lexer;
	t_node		*node;
	t_parser	*parser;
	char		*line;

	atexit(check_leaks); // TODO - Remove this

	t_visitor *free_visitor = freeing_visitor();
	t_visitor *printer_visitor = visualizer_visitor();
	// t_visitor *visiter_visitor = interpreter_visitor();

	while (true)
	{
		line = readline("$> ");
		if (line == NULL)
			break ;
		lexer = lexer_new(line);
		tokens = lexer_tokenize(lexer);
		if (tokens->size > 1)
		{
			// TODO
			// for (size_t i = 0; i < tokens->size; i++) {
			// 	printf("%s|%d\n", tokens->tokens[i].lexeme, tokens->tokens[i].type);
			// }
			parser = parser_new(tokens);
			node = parse(parser);

			visit_node(printer_visitor, node);
			printf("\n");
			visit_node(free_visitor, node);
			parser_free(&parser);
		}
		tokens_free(&tokens);
		lexer_free(&lexer);
	}

	visitor_free(&printer_visitor);
	visitor_free(&free_visitor);
	// visitor_free(&visiter_visitor);
}

/*

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
