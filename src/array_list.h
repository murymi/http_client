#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct string_array_list strAL;
struct string_array_list
{
    size_t size;
    size_t capacity;
    char **list;
};

strAL *string_array_list_create();

void string_array_list_add_one(strAL *sal);

void string_array_list_append(strAL *al, char *str);

void string_array_list_print(strAL *a);

char *string_array_list_get(strAL *a, size_t pos);

char *string_array_list_get_copy(strAL *a, size_t pos);

char **string_array_list_get_array(strAL *sal, size_t start, size_t end);

void string_array_list_add(strAL *sal, char *value, size_t pos);

void string_arraylist_destroy(strAL **sal);

void string_array_list_delete(strAL *sal, size_t pos);

inline size_t string_array_list_size(strAL *sal){
    return sal->size;
}

inline size_t string_array_list_capacity(strAL *sal){
    return sal->capacity;
}

strAL *string_array_list_create_capacity(size_t capacity);

void string_arraylist_destroy(strAL **sal);

void string_array_list_append_allocated(strAL *al, char *str);