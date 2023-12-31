#pragma once

#include "map.h"
#include "_string.h"
#include "regex.h"
#include "array_list.h"

/*A simple url parser*/
typedef struct url url_t;
struct url{
    char *scheme;
    char *domain;
    char *www_domain;
    char *path;
    char *port;
    map_t * params;
};

url_t *url_parser_parse(char *url);

void url_free(url_t *url);

struct map_t *parse_url(char *url);

struct map_t *parse_url_query(char *query);

char *get_param_from_url(char *url,char *key);

char *get_domain_name_from_url(char *url);

char *get_scheme_from_url(char *url);

char *get_port_from_url(char *url);

char *get_path_from_url(char *url);

bool verify_url(char *url);

char *get_param_part_from_url(char *url);