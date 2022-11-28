/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_5.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 18:18:15 by soubella          #+#    #+#             */
/*   Updated: 2022/11/26 15:55:56 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "string_builder.h"
#include "lexer_utils.h"
#include "ft_printf.h"
#include "parser.h"
#include "main.h"

static t_elements	*resolve_vars(char *line)
{
	t_string_builder	*builder;
	t_elements			*elements;

	if (string_index_of(line, "$") == -1)
	{
		elements = elements_new_cap(1);
		elements_add(elements,
			string_create(line, true), WORD_ELEMENT, false);
		return (elements);
	}
	elements = elements_new();
	builder = string_builder_new_cap(string_length(line));
	while (*line)
	{
		if (*line != '$' || (*line == '$' && !is_identifier_start(line[1])))
		{
			string_builder_append_char(builder, *line++);
			continue ;
		}
		line = create_var(elements, line + 1, builder);
	}
	create_var(elements, line + 1, builder);
	string_builder_free(&builder);
	free(line);
	return (elements);
}

static void	restore_stdin(bool *success)
{
	dup2(g_globals.in_fd, STDIN_FILENO);
	close(g_globals.in_fd);
	g_globals.in_fd = -1;
	*success = false;
}

static char	*stdin_read(char *limiter, bool *success)
{
	char	*line;

	g_globals.in_fd = -1;
	if (isatty(STDIN_FILENO))
		line = readline("> ");
	else
		line = readline(NULL);
	if (g_globals.in_fd != -1 || line == NULL)
	{
		if (g_globals.in_fd != -1)
			restore_stdin(success);
		free(line);
		return (NULL);
	}
	if (string_equals(limiter, line))
	{
		free(line);
		return (NULL);
	}
	return (line);
}

static void	process_line(t_elements *elements, char *line, bool expand_vars)
{
	t_elements	*temp;

	if (expand_vars)
	{
		temp = resolve_vars(string_join(line, "\n"));
		elements_fill(elements, &temp);
	}
	else
		elements_add(elements,
			string_create(string_join(line, "\n"), true), WORD_ELEMENT, false);
	free(line);
}

t_optional_elements	read_from_stdin(char *limiter, bool expand_vars)
{
	t_elements	*elements;
	bool		success;
	char		*line;

	signal(SIGINT, sigint_heredoc_handler);
	elements = elements_new();
	success = true;
	line = NULL;
	while (true)
	{
		g_globals.in_fd = -1;
		line = stdin_read(limiter, &success);
		if (line == NULL)
			break ;
		if (!isatty(STDIN_FILENO))
			ft_printf(STDOUT_FILENO, "\033[1A\033[K");
		process_line(elements, line, expand_vars);
	}
	signal(SIGINT, sigint_default_handler);
	return (elements_optional(elements, success));
}
