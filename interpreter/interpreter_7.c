/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter_7.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 21:04:25 by soubella          #+#    #+#             */
/*   Updated: 2022/12/06 14:12:29 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

#include "string_utils.h"
#include "interpreter.h"
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
		error("Couldn't fork a child process");
	if (pid == 0)
		exit(builtin->block(env, size, args));
	return (result_create(PID, pid));
}
