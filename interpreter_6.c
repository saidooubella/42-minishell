/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_6.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 11:17:22 by soubella          #+#    #+#             */
/*   Updated: 2022/12/02 16:00:40 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>

#include "string_utils.h"
#include "interpreter.h"
#include "ft_printf.h"

t_to_be_closed	*to_be_closed_new(void)
{
	t_to_be_closed	*element;

	element = malloc(sizeof(t_to_be_closed));
	element->fds = malloc(sizeof(int) * 8);
	element->capacity = 8;
	element->size = 0;
	return (element);
}

void	to_be_closed_add(t_to_be_closed	*element, int fd)
{
	int		*new_fds;
	size_t	new_capacity;

	if (element->size + 1 >= element->capacity)
	{
		new_capacity = (element->size + 1) << 1;
		new_fds = malloc(sizeof(int) * new_capacity);
		if (new_fds == NULL)
			memory_error();
		bytes_copy(new_fds, element->fds,
			element->size * sizeof(int));
		free(element->fds);
		element->capacity = new_capacity;
		element->fds = new_fds;
	}
	element->fds[element->size++] = fd;
}

void	resolve_program_io(
	t_environment *env, t_command_node *node, int in, int out)
{
	int	old_out;
	int	old_in;

	old_out = out;
	old_in = in;
	redirect_fd(out, STDOUT_FILENO);
	redirect_fd(in, STDIN_FILENO);
	resolve_redirections(env, node, &in, &out);
	if (out != old_out)
		redirect_fd(out, STDOUT_FILENO);
	if (in != old_in)
		redirect_fd(in, STDIN_FILENO);
}

bool	is_file_ambiguous(t_elements *elements, t_environment *env)
{
	t_string	temp;
	bool		result;

	result = false;
	if (elements->size == 1 && elements->elements->type == VAR_ELEMENT)
	{
		temp = elements_resolve(elements, env);
		if (string_is_blank(temp.value) || has_multiple_parts(temp.value))
		{
			ft_printf(STDERR_FILENO,
				"minishell: $%s: ambiguous redirect\n",
				elements->elements->value);
			result = true;
		}
		string_free(&temp);
	}
	return (result);
}

bool	is_empty_argument(t_elements *elements, t_environment *env)
{
	return (elements->size == 1
		&& elements->elements->type == VAR_ELEMENT
		&& string_is_blank(env_get_var(env,
				elements->elements->value.value, NULL)));
}
