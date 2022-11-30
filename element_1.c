/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   element_1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soubella <soubella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 14:35:12 by soubella          #+#    #+#             */
/*   Updated: 2022/11/30 21:47:47 by soubella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "environment.h"
#include "element.h"
#include "string_builder.h"
#include "string_utils.h"
#include "wildcard_matcher.h"
#include "utils.h"

t_elements	*elements_new(void)
{
	return (elements_new_cap(16));
}
#include <unistd.h>
#include "ft_printf.h"
void	handle_variable(
	t_environment *env, t_string_builder *builder, t_element *var)
{
	char	*exit_code;
	char	*value;

	if (string_equals(var->value.value, "?"))
	{
		exit_code = llong_to_string(env->exit_code);
		string_builder_append_cstring(builder, exit_code);
		free(exit_code);
	}
	else
	{
		value = env_get_var(env, var->value.value, "");
		if (!var->in_string)
			value = string_trim(value);
		string_builder_append_cstring(builder, value);
		if (!var->in_string)
			free(value);
	}
}

t_string	elements_resolve_epilogue(
	t_elements *elements, t_environment *env, t_string_builder *builder)
{
	t_string	expanded;
	char		*result;

	result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	if (!elements->expandable)
		return (string_create(result, true));
	if (env->is_bonus)
	{
		expanded = apply_pattern(result, env->working_dir.value);
		if (expanded.value != result)
			free(result);
	}
	else
		expanded = string_create(result, true);
	return (expanded);
}

t_string	elements_resolve(t_elements *elements, t_environment *env)
{
	t_string_builder	*builder;
	size_t				index;
	t_element			*temp;

	builder = string_builder_new();
	index = -1;
	while (++index < elements->size)
	{
		temp = &elements->elements[index];
		if (temp->type == VAR_ELEMENT && temp->expandable)
			handle_variable(env, builder, temp);
		else
			string_builder_append_cstring(builder, temp->value.value);
	}
	return (elements_resolve_epilogue(elements, env, builder));
}

t_optional_elements	elements_optional(t_elements *elements, bool present)
{
	t_optional_elements	optional;

	optional.present = present;
	optional.elements = elements;
	return (optional);
}
