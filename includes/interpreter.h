/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 10:40:35 by soubella          #+#    #+#             */
/*   Updated: 2022/12/29 18:31:20 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERPRETER_H
# define INTERPRETER_H

# include <string.h>

# include "string_utils.h"
# include "environment.h"
# include "free_aware_string.h"
# include "utils.h"
# include "nodes.h"

# define BUILTINS_COUNT 7

typedef enum e_result_type
{
	EXIT_STATUS,
	ERROR,
	PID,
}	t_result_type;

typedef struct s_result
{
	t_result_type	type;
	int				extra;
}	t_result;

typedef struct s_visit_extras
{
	int		in;
	int		out;
	bool	should_wait;
	bool	enforce_fork;
}	t_visit_extras;

typedef struct s_to_be_closed
{
	int		*fds;
	size_t	capacity;
	size_t	size;
}	t_to_be_closed;

void			to_be_closed_add(t_to_be_closed	*element, int fd);
t_to_be_closed	*to_be_closed_new(void);

t_result		await_process(t_result result);
bool			redirect_fd(int src, int dst);
int				duplicate_fd(int fd);
void			close_fd(int fd);

bool			has_multiple_parts(char *string);
bool			is_empty_argument(t_elements *elements, t_environment *env);
bool			is_file_ambiguous(t_elements *elements, t_environment *env);
bool			resolve_program_io(
					t_environment *env, t_command_node *node,
					int in, int out);

char			**unwrap_args(
					t_environment *env, t_command_node *node,
					size_t *actual_size, bool unwrap_all);
char			**unwrap_env(t_symbol *syms, size_t size);
t_result		forked_builtin(
					t_environment *env, t_builtin *builtin,
					char **args, size_t size);

bool			resolve_redirections(
					t_environment *env, t_command_node *node);

t_result		visit_parent_node(
					t_environment *env, t_parent_node *node,
					t_to_be_closed *tbc, t_visit_extras extra);
t_result		visit_pipe_node(
					t_environment *env, t_pipe_node *node,
					t_to_be_closed *tbc, t_visit_extras extra);
t_result		visit_conjuction_node(
					t_environment *env, t_conjuction_node *node,
					t_to_be_closed *tbc, t_visit_extras extra);
t_result		result_create(t_result_type type, int extra);
t_result		visit_command_node(
					t_environment *env, t_command_node *node,
					t_to_be_closed *tbc, t_visit_extras extra);
t_visit_extras	visit_extras(int in, int out,
					bool should_wait, bool enforce_fork);
t_result		visit_node_internal(
					t_environment *env, t_node *node,
					t_to_be_closed *tbc, t_visit_extras extra);
t_result		visit_node(t_environment *env, t_node *node);

#endif
