//
// Created by pavlo on 10.01.25.
//

#pragma once

#include <stdlib.h>
#include <assert.h>
#include <errno.h>

// List initializes a new list-type with the provided type as an anonymous structure.
// Therefore, it may be a good idea to typedef it.
#define List(T) struct { size_t len; size_t cap; T* ptr; }

// NEWLIST is an initializer of an empty list of type T and capable of containing `size`
// elements before reallocations.
#define NEWLIST(T, size) { .len = 0, .cap = size, .ptr = malloc(size*sizeof(T)) }

// PUSH appends a new value to the end of a list and reallocates if new space is needed.
// Cannot be used as an expression.
#define LIST_PUSH(list, elem)                  \
{                                         \
    if (list.len >= list.cap) LIST_GROW_(list); \
    list.ptr[list.len++] = elem;          \
}

// POP removes the last element in a list and returns it back. Can be used as an expression.
#define LIST_POP(list) (list.ptr[--list.len])

// LIST_GROW_ increases the list's capacity further. Intended for internal use by PUSH primarily.
// Cannot be used as an expression.
//
// Note: if realloc fails, an assertation is raised. However, if NDEBUG is defined, list.ptr
// will be silently be set to NULL, therefore next list access most likely results in segfault.
#define LIST_GROW_(list)                                                 \
{                                                                  \
    const size_t threshold = 256;                                  \
    size_t nextcap;                                                \
    if (list.cap < threshold)                                      \
        nextcap = (list.cap|1)*2;                                  \
    else                                                           \
        nextcap = list.cap + ((list.cap + 3*threshold) >> 2);      \
    void* newptr = realloc(list.ptr, nextcap*sizeof(list.ptr[0])); \
    assert(list.cap < nextcap /*next capacity overflow*/);         \
    assert(newptr != NULL /*realloc failed*/);                     \
    list.ptr = newptr;                                             \
    list.cap = nextcap;                                            \
}

// LIST_FREE deallocates a list by freeing its pointer. Can be used as an expression.
#define LIST_FREE(list) free(list.ptr)
