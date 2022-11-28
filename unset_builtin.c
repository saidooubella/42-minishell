/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 17:20:34 by soubella          #+#    #+#             */
/*   Updated: 2022/11/25 17:51:28 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include  <stddef.h>
#include  <unistd.h>

#include "environment.h"
#include "lexer_utils.h"
#include "ft_printf.h"

int	unset_builtin(t_environment *env, size_t argc, char **argv)
{
	size_t	index;
	char	*arg;
	size_t	jndex;

	index = 0;
	while (++index < argc)
	{
		arg = argv[index];
		jndex = 0;
		if (is_identifier_start(arg[jndex]))
			while (is_identifier_cont(arg[jndex]))
				jndex++;
		if (arg[jndex] != '\0')
		{
			ft_printf(STDERR_FILENO,
				"export: `%s': not a valid identifier\n", arg);
			return (1);
		}
		env_remove_var(env, arg);
	}
	return (0);
}
