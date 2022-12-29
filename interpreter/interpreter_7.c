/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_7.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 21:04:25 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 17:30:09 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

#include "string_utils.h"
#include "interpreter.h"
#include "ft_printf.h"
#include "builtins.h"
#include "utils.h"

bool	has_multiple_parts(char *string)
{
	char	**splitted;
	bool	result;
	size_t	size;

	splitted = string_split(string, " \t\v\n\r\f");
	result = false;
	size = 0;
	while (splitted[size])
		size++;
	if (size != 1)
		result = true;
	size = -1;
	while (splitted[++size])
		free(splitted[size]);
	free(splitted);
	return (result);
}

t_result	forked_builtin(
	t_environment *env, t_builtin *builtin, char **args, size_t size)
{
	int	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_printf(STDOUT_FILENO, "minishell: Couldn't fork a child process\n");
		return (result_create(ERROR, 0));
	}
	if (pid == 0)
		exit(builtin->block(env, size, args));
	return (result_create(PID, pid));
}
