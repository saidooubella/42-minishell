#include <stdlib.h>

#include "environment.h"
#include "element.h"
#include "string_builder.h"
#include "string_utils.h"
#include "wildcard_matcher.h"
#include "utils.h"

t_elements	*elements_new_cap(size_t capacity)
{
	t_elements	*elements;

	elements = malloc(sizeof(t_elements));
	if (elements == NULL)
		memory_error();
	elements->elements = malloc(sizeof(t_element) * capacity);
	if (elements->elements == NULL)
		memory_error();
	elements->capacity = capacity;
	elements->size = 0;
	return (elements);
}

t_elements	*elements_new(void)
{
	return (elements_new_cap(16));
}

t_string	elements_resolve(t_elements *elements, t_environment *env)
{
	t_string 			expanded;
	t_string_builder	*builder;
	char				*result;
	size_t				index;

	builder = string_builder_new();
	index = -1;
	while (++index < elements->size)
	{
		t_element *temp = &elements->elements[index];
		if (temp->type == VAR_ELEMENT && temp->expandable)
			string_builder_append_cstring(builder, env_get_var(env, temp->value.value, ""));
		else
			string_builder_append_cstring(builder, temp->value.value);
	}
	result = string_builder_to_cstr(builder);
	string_builder_free(&builder);
	if (!elements->expandable)
		return (string_create(result, true));
	expanded = apply_pattern(result, env->working_dir.value);
	if (expanded.value != result)
		free(result);
	return (expanded);
}

t_optional_elements	elements_optional(t_elements *elements, bool present)
{
	t_optional_elements	optional;

	optional.present = present;
	optional.elements = elements;
	return (optional);
}

void	elements_free(t_elements **elements)
{
	size_t	index;

	index = -1;
	while (++index < (*elements)->size)
		string_free(&(*elements)->elements[index].value);
	free((*elements)->elements);
	free((*elements));
	*elements = NULL;
}

void	elements_fill(t_elements *dst, t_elements **src)
{
	t_element	*temp;
	size_t		index;

	index = 0;
	while (index < (*src)->size)
	{
		temp = &(*src)->elements[index++];
		elements_add(dst, temp->value, temp->type, temp->expandable);
	}
	free((*src)->elements);
	free((*src));
	*src = NULL;
}

void	elements_add(t_elements *elements, t_string value, t_element_type type, bool expandable)
{
	size_t		new_capacity;
	t_element	*new_elements;

	if (elements->size + 1 >= elements->capacity)
	{
		new_capacity = (elements->size + 1) << 1;
		new_elements = malloc(sizeof(t_element) * new_capacity);
		if (new_elements == NULL)
			memory_error();
		bytes_copy(new_elements, elements->elements, elements->size * sizeof(t_element));
		free(elements->elements);
		elements->capacity = new_capacity;
		elements->elements = new_elements;
	}
	elements->elements[elements->size].expandable = expandable;
	elements->elements[elements->size].value = value;
	elements->elements[elements->size].type = type;
	elements->size++;
}
