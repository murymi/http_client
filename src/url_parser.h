#pragma once 

#include <math.h>
#include "array_list.h"
#include "map.h"
#include <stdbool.h>
#include <regex.h>

typedef struct url url;

struct url {
    char *raw_url;
    char *scheme;
    char *path;
    char *port;
    char *address;
    map_t *params;
    strAL *flags;
    char *route;
};


url *url_parse(char *url);

bool verify_url(char *url);


#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _string_t _string_t;

struct _string_t {
    char *ptr;

    // position of null ptr
    size_t len;

    // allocated size
    size_t size;
};

_string_t *_string_create();

_string_t *_string_create_size(size_t size);

void _string_append(_string_t *s, char c);

void _string_concat(_string_t *s, char *str);

void _string_destroy(_string_t *s);

void string_expand(_string_t *s, size_t capacity);

strAL *_split(char *str, char *delim);

char *_string_create_copy(char *str);

strAL *_split_lim(char *str, char *delim, int lim);

size_t _find_first_not_space(char *str);

size_t _find_last_not_space(char *str);

char *_trim(char *buffer);

char *_string_sub(size_t start, size_t stop, char *buffer);

char *replace_html_entity(char *str);