/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:31:19 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 21:29:04 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "environment.h"
#include "ft_printf.h"
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
	if (!redirect_fd(in, STDIN_FILENO))
		return (false);
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
	if (!redirect_fd(out, STDOUT_FILENO))
		return (false);
	string_free(&path);
	return (true);
}

static bool	redirect_heredoc(
	t_environment *env, t_elements *elements)
{
	int			read_write[2];
	t_string	content;
	int			in;

	if (pipe(read_write) == -1)
	{
		ft_printf(STDOUT_FILENO, "minishell: Failed to create a pipe\n");
		return (false);
	}
	content = elements_resolve(elements, env);
	write(read_write[1], content.value, string_length(content.value));
	close_fd(read_write[1]);
	in = read_write[0];
	if (!redirect_fd(in, STDIN_FILENO))
		return (false);
	string_free(&content);
	return (true);
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
	if (!redirect_fd(out, STDOUT_FILENO))
		return (false);
	string_free(&path);
	return (true);
}

bool	resolve_redirections(
	t_environment *env, t_command_node *node)
{
	t_redirection	redirect;
	size_t			index;
	bool			res;

	index = -1;
	while (++index < node->redirections_size)
	{
		redirect = node->redirections[index];
		if (redirect.type == INPUT)
			res = redirect_in(env, redirect.elements);
		else if (redirect.type == OUTPUT)
			res = redirect_out(env, redirect.elements);
		else if (redirect.type == HEREDOC)
			res = redirect_heredoc(env, redirect.elements);
		else if (redirect.type == APPEND)
			res = redirect_append_out(env, redirect.elements);
		else
			error("Illegal state in 'resolve_redirections'");
		if (!res)
			return (false);
	}
	return (true);
}
