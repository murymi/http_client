#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <array_list.h>


typedef struct string_t string_t;
struct string_t
{
    size_t size;
    char *chars;
};

string_t *string_create();

string_t *string_create_from_string(char *string);

bool string_append(string_t *str, char c);

bool string_destroy(string_t *str);

bool string_pop(string_t *str);

char string_back(string_t *str);

char string_front(string_t *str);

void string_concat(string_t *dest, char *src, size_t len);

char *string_removechar(char c, char *buffer, size_t len);

char *string_replacechar(char previous, char current, char *buffer, size_t len);

char *string_removechar_at(unsigned int pos, char *buffer, size_t len);

int string_last_index_of(char c, char *str, unsigned int len);

char *string_sub(unsigned int start, unsigned int stop, char *buffer, unsigned int len);

bool endswith(char c, char *str);

bool startswith(char c, char *str);

strAL *split(char delim, char *str, size_t str_size);

strAL *split_lim(char delim, char *str, size_t str_size, int lim);

char *setchar_at(char *str, char c, size_t pos, size_t len);

bool starts_with_word(char *word,
                      char *str);
char *string_create_copy(char *str);

char *string_cover(char *str);

char *trim(char *str);

size_t find_last_not_space(char *str);

size_t find_first_not_space(char *str);

char *string_to_lower(char *str);

char *string_add_char(char *str,char c,size_t index);