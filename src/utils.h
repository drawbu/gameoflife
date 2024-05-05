#pragma once

#include <stddef.h>

#define LENOF(x) (sizeof (x) / sizeof *(x))
#define DEF_ARR(type) struct { size_t size, alloc; type *arr; }

typedef DEF_ARR(char) char_array_t;

void *append_to_array(void *array, const void *elem, size_t size);
void remove_from_array(void *array, size_t idx, size_t size);
