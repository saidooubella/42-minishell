/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:31:19 by soubella          #+#    #+#             */
/*   Updated: 2022/12/03 15:41:32 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "environment.h"
#include "interpreter.h"
#include "parser.h"

static bool	redirect_in(
	t_environment *env, t_elements *elements)
{
	t_string	path;
	int			in;

	if (is_file_ambiguous(elements, env))
		return (false);
	path = elements_resolve(elements, env);
	in = open(path.value, O_RDONLY);
	if (in == -1)
		error("minishell: %s: %s", path.value, strerror(errno));
	redirect_fd(in, STDIN_FILENO);
	string_free(&path);
	return (true);
}

static bool	redirect_out(
	t_environment *env, t_elements *elements)
{
	t_string	path;
	int			out;

	if (is_file_ambiguous(elements, env))
		return (false);
	path = elements_resolve(elements, env);
	out = open(path.value, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (out == -1)
		error("minishell: %s: %s", path.value, strerror(errno));
	redirect_fd(out, STDOUT_FILENO);
	string_free(&path);
	return (true);
}

static void	redirect_heredoc(
	t_environment *env, t_elements *elements)
{
	int			read_write[2];
	t_string	content;
	int			in;

	if (pipe(read_write) == -1)
		error("Failed to create a pipe");
	content = elements_resolve(elements, env);
	write(read_write[1], content.value, string_length(content.value));
	close_fd(read_write[1]);
	in = read_write[0];
	redirect_fd(in, STDIN_FILENO);
	string_free(&content);
}

static bool	redirect_append_out(
	t_environment *env, t_elements *elements)
{
	t_string	path;
	int			out;

	if (is_file_ambiguous(elements, env))
		return (false);
	path = elements_resolve(elements, env);
	out = open(path.value, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (out == -1)
		error("minishell: %s: %s", path.value, strerror(errno));
	redirect_fd(out, STDOUT_FILENO);
	string_free(&path);
	return (true);
}

bool	resolve_redirections(
	t_environment *env, t_command_node *node)
{
	t_redirection	redirect;
	size_t			index;

	index = -1;
	while (++index < node->redirections_size)
	{
		redirect = node->redirections[index];
		if (redirect.type == INPUT)
		{
			if (!redirect_in(env, redirect.elements))
				return (false);
		}
		else if (redirect.type == OUTPUT)
		{
			if (!redirect_out(env, redirect.elements))
				return (false);
		}
		else if (redirect.type == HEREDOC)
			redirect_heredoc(env, redirect.elements);
		else if (redirect.type == APPEND)
			if (!redirect_append_out(env, redirect.elements))
				return (false);
	}
	return (true);
}
