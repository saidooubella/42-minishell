/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:31:19 by soubella          #+#    #+#             */
/*   Updated: 2022/11/26 13:48:37 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "environment.h"
#include "interpreter.h"
#include "parser.h"

static void	redirect_in(
	t_environment *env, t_elements *elements, int *in)
{
	t_string	path;

	close_fd(*in);
	path = elements_resolve(elements, env);
	*in = open(path.value, O_RDONLY);
	if (*in == -1)
		error("minishell: %s: %s", strerror(errno), path);
	string_free(&path);
}

static void	redirect_out(
	t_environment *env, t_elements *elements, int *out)
{
	t_string	path;

	close_fd(*out);
	path = elements_resolve(elements, env);
	*out = open(path.value, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (*out == -1)
		error("minishell: %s: %s", strerror(errno), path);
	string_free(&path);
}

static void	redirect_heredoc(
	t_environment *env, t_elements *elements, int *in)
{
	int			read_write[2];
	t_string	content;

	close_fd(*in);
	if (pipe(read_write) == -1)
		error("Failed to create a pipe");
	content = elements_resolve(elements, env);
	write(read_write[1], content.value, string_length(content.value));
	close_fd(read_write[1]);
	*in = read_write[0];
	string_free(&content);
}

static void	redirect_append_in(
	t_environment *env, t_elements *elements, int *out)
{
	t_string	path;

	close_fd(*out);
	path = elements_resolve(elements, env);
	*out = open(path.value, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (*out == -1)
		error("minishell: %s: %s", strerror(errno), path.value);
	string_free(&path);
}

void	resolve_redirections(
	t_environment *env, t_command_node *node, int *in, int *out)
{
	t_redirection	redirect;
	size_t			index;

	index = -1;
	while (++index < node->redirections_size)
	{
		redirect = node->redirections[index];
		if (redirect.type == INPUT)
			redirect_in(env, redirect.elements, in);
		else if (redirect.type == OUTPUT)
			redirect_out(env, redirect.elements, out);
		else if (redirect.type == HEREDOC)
			redirect_heredoc(env, redirect.elements, in);
		else if (redirect.type == APPEND)
			redirect_append_in(env, redirect.elements, out);
	}
}
