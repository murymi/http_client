#include "url_parser.h"

#include "map.h"
#include "_string.h"
#include "regex.h"
#include "array_list.h"

url_t *url_parser_parse(char *url)
{
    //if(!verify_url(url)) {
    //    puts("invalid url");
    //    return NULL;
    //}

    url_t * url_obj = malloc(sizeof(url_t));
    
    /*url_obj->domain = NULL;
    url_obj->params = NULL;
    url_obj->scheme = NULL;
    url_obj->port = NULL;
    url_obj->path = NULL;*/

    memset(url_obj,0,sizeof(url_t));

    url_obj->domain = get_domain_name_from_url(url);
    url_obj->path = get_path_from_url(url);

    char *p = get_param_part_from_url(url);

    url_obj->params = parse_url_query(p);

    url_obj->port = get_port_from_url(url);

    string_t * ww = string_create_from_string("www.");
    string_concat(ww,url_obj->domain,strlen(url_obj->domain));

    url_obj->www_domain = ww->chars;

    url_obj->scheme = get_scheme_from_url(url);

    free(ww);

    free(p);

    return url_obj;
}

void url_free(url_t *url)
{
    if(url == NULL) return;

    if(url->domain) free(url->domain);
    if(url->params) map_destroy(&(url->params));
    if(url->path) free(url->path);
    if(url->port) free(url->port);
    if(url->scheme) free(url->scheme);
}


struct map_t *parse_url_query(char *query)
{
    if(query == NULL) return NULL;

    if(strlen(query) < 2) return NULL;

    strAL *param_parts = split('&',
                                query,
                                strlen(query));

    if (param_parts->size == 0)
        return NULL;

    strAL *tmp = param_parts;

    map_t *params = map_create();

    for(size_t i = 0; i < tmp->size; i++)
    {
        char *toProcess = string_array_list_get(tmp, i);
        strAL *this_pair = split_lim('=', toProcess, strlen(toProcess), 2);

        if (this_pair->size != 2)
        {
            continue;
        }

        if (strlen(string_array_list_get(this_pair, 0)) <= 0 || strlen(string_array_list_get(this_pair, 1)) <= 0)
        {
            string_arraylist_destroy(&this_pair);

            continue;
        }

        map_put(params, string_array_list_get(this_pair, 0), string_array_list_get(this_pair, 1));

        string_arraylist_destroy(&this_pair);

    }

    string_arraylist_destroy(&param_parts);
    return params;
}

struct map_t *parse_url(char *url)
{
    strAL *url_parts = split_lim('?', url, strlen(url), 2);

    if (url_parts->size != 2)
    {
        return NULL;
    }

    map_t *my_parts = map_create();
    map_put(my_parts, "url", string_array_list_get(url_parts, 0));
    map_put(my_parts, "query", string_array_list_get(url_parts, 1));

    string_arraylist_destroy(&url_parts);
    return my_parts;
}

char *get_param_part_from_url(char *url)
{
    strAL *url_parts = split_lim('?', url, strlen(url), 2);

    if (url_parts->size != 2)
    {
        return NULL;
    }

    char *ret = string_array_list_get_copy(url_parts, 1);

    string_arraylist_destroy(&url_parts);
    return ret;
}

char *get_param_from_url(char *url, char *key)
{
    map_t *url_m = parse_url(url);
    if (url_m == NULL)
    {
        return NULL;
    }

    if (url_m->size != 2)
    {
        map_destroy(&url_m);
        return NULL;
    }

    map_t *params = parse_url_query(map_get(url_m, "query"));

    if (params == NULL)
    {
        return NULL;
    }

    char *value = map_get(params, key);

    map_destroy(&url_m);
    map_destroy(&params);

    return value;
}

char *get_scheme_from_url(char *url)
{
    strAL *colon_splits = split(':', url, strlen(url));

    char *scheme = NULL;

    if (colon_splits->size > 0)
        scheme = string_array_list_get_copy(colon_splits, 0);

    string_arraylist_destroy(&colon_splits);

    return scheme;
}

char *get_domain_name_from_url(char *url)
{
    strAL *colon_splits = split(':', url, strlen(url));
    strAL *slash_splits = NULL;

    char *uri = NULL;

    if (colon_splits->size > 1)
    {

        char *domain_double_slashed = string_array_list_get_copy(colon_splits, 1);
        int dds_len = strlen(domain_double_slashed);

        slash_splits = split('/', domain_double_slashed, dds_len);

        if (slash_splits)
        {

            uri = string_array_list_get_copy(slash_splits, 2);
        }

        string_arraylist_destroy(&slash_splits);
    }

    string_arraylist_destroy(&colon_splits);

    return uri;
}

// http://localhost:2000/bla/bla
char *get_port_from_url(char *url)
{
    strAL *colon_splits = split_lim(':', url, strlen(url), 3);

    if (colon_splits->size != 3)
    {
        string_arraylist_destroy(&colon_splits);
        return NULL;
    }

    char *port = NULL;

    char *last_part = string_array_list_get_copy(colon_splits, 2);

    strAL *slash_splits = split('/', last_part, strlen(last_part));

    if (slash_splits->size > 0){
        port = string_array_list_get_copy(slash_splits, 0);
    }

    string_arraylist_destroy(&slash_splits);
    string_arraylist_destroy(&colon_splits);

    return port;
}


char *get_path_from_url(char *url)
{
    strAL *colon_splits = split_lim(':', url, strlen(url), 3);

    int c_split_len = colon_splits->size;

    char *path = NULL;

    char *last_part = string_array_list_get_copy(colon_splits, c_split_len - 1); //1

    strAL *slash_splits = NULL;

    if (c_split_len == 3)
    {
        slash_splits = split_lim('/', last_part, strlen(last_part), 2);
        path = string_array_list_get_copy(slash_splits, 1);
    }
    else if (c_split_len == 2)
    {
        slash_splits = split_lim('/', last_part, strlen(last_part), 4);
        path = string_array_list_get_copy(slash_splits, 3);
    }

    string_arraylist_destroy(&slash_splits);
    string_arraylist_destroy(&colon_splits);

    if(path == NULL) return path;

    if(strlen(path) == 0){
        free(path);
        path = NULL;

        string_t * t = string_create_from_string("/");
        path = string_create_copy(t->chars);
        string_destroy(t);
        return path;
    }

    char *pathtmp = path;

    path = string_add_char(pathtmp,'/',0);

    free(pathtmp);

    return path;
}


// http://localhost/bla/bla
bool verify_url(char *url)
{
    regex_t regex;
    int reti;
    bool out = false;

    reti = regcomp(&regex, "((http|https)://)[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)", REG_EXTENDED);
    if (reti) {
        return false;
    }

    reti = regexec(&regex, url, 0, NULL, 0);
    if (!reti) {
        out = true;
    }

    regfree(&regex);

    return out;

}

char *replace_html_entity(char *str)
{
    size_t len = strlen(str);
    string_t *decoded = string_create();

    char *x = NULL;
    int offset = 0;

    while ((x = strchr(str, '&')) != NULL)
    {
        char *y = strchr(str, ';');

        int s_len;

        if (y != NULL)
        {

            char entity[10] = {0};

            int ent_len = y - x + 1;

            int startPos = x - str;

            strncpy(entity, x, ent_len);
            entity[ent_len] = '\0';

            string_concat(decoded, str, startPos);

            s_len = startPos + ent_len;
            offset += s_len;
            str += s_len;

            if(strncmp(entity, "&nbsp;", ent_len) == 0) {
                string_append(decoded, ' ');
            } else if (strncmp(entity, "&lt;", ent_len) == 0) {
                string_append(decoded, '<');
            } else if (strncmp(entity, "&gt;", ent_len) == 0) {
                string_append(decoded, '>');
            } else if (strncmp(entity, "&amp;", ent_len) == 0) {
                string_append(decoded, '&');
            } else if (strncmp(entity, "&quot;", ent_len) == 0) {
                string_append(decoded, '\"');
            } else if (strncmp(entity, "&apos;", ent_len) == 0) {
                string_append(decoded, '\'');
            }

            continue;
        } 

        s_len = x - str + 1;
        string_concat(decoded, str, s_len);
        str += (s_len);
        offset += s_len;
    }

    string_concat(decoded, str, len - offset);
    
    char *r = decoded->chars;
    free(decoded);
    return r;
}
