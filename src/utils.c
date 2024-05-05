#include "utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *append_to_array(void *array, const void *elem, size_t size)
{
    DEF_ARR(uint8_t) *arr = array;

    if (arr->size + 1 > arr->alloc) {
        arr->alloc = (arr->alloc) ? arr->alloc * 2 : 2;
        void *ptr = reallocarray(arr->arr, arr->alloc + 1, size);
        if (ptr == NULL)
            return NULL;
        arr->arr = ptr;
    }
    memcpy(arr->arr + (arr->size * size), elem, size);
    arr->size += 1;
    return array;
}

void remove_from_array(void *array, size_t idx, size_t size)
{
    DEF_ARR(uint8_t) *arr = array;

    if (idx >= arr->size)
        return;
    arr->size -= 1;
    if (idx != arr->size)
        memcpy(arr->arr + (idx * size), arr->arr + (arr->size * size), size);
    memset(arr->arr + (arr->size * size), 0, size);
}
