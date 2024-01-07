
#include "url_parser.h"

int min(int a, int b) {
    if(a < b) {
        return a;
    } else {
        return b;
    }
}

// http://localhost:3000;
url *url_parse(char *str) {

    if(!verify_url(str)) {
        return NULL;
    }

    url *parsed = malloc(sizeof(url));

    char *u = trim(str);

    parsed->raw_url = u;

    char *s = strstr(u, "://");

    if(s) {
        char  *ret = _string_sub(0, s - u, u);
        parsed->scheme = ret;
    } else {
        parsed->scheme = NULL;
    }

    s = s + 3;
    char *sl = NULL;

    {
        sl = strstr(s + 3, "/");

        char *ret = NULL;

        if(sl) {
           ret = _string_sub(0, sl - s, s);

           parsed->route = string_create_copy(sl);
        } else {
            ret = _string_sub(0, strlen(s), s);
            parsed->path = string_create_copy("/");

            /*work around*/
            parsed->route = string_create_copy("/");
        }

        {
            strAL *addr_splits = _split_lim(ret, ":", 2);
            parsed->port = 
            addr_splits->size == 2 ? string_array_list_get(addr_splits, 1) : NULL;

            parsed->address = string_array_list_get(addr_splits, 0);

            free(addr_splits);
            free(addr_splits->list);
        }


        free(ret);

    }

    char *p_start = NULL;
    {
        if(sl) {
             p_start = strchr(sl, '?');
        }
    }
            //parsed->path = p_start ? string_sub(0, p_start - sl, sl) : string_create_copy(sl);
    char *frag_start = NULL;

    if(sl) {
        frag_start = strchr(sl, '#');

        if(frag_start) {
            parsed->flags = _split(frag_start + 1, "#");
        }
    }

    char *param_string = NULL;
    int p2f, p2p;
    if(p_start && frag_start) {
        p2f = frag_start - sl;
        p2p = p_start - sl;
        
        int path_size = min(p2f, p2p);

        parsed->path = _string_sub(0,path_size, sl);
        
        assert(path_size == p2p);

        param_string = _string_sub(0, frag_start - p_start - 1, p_start + 1);

    } else if(p_start && !frag_start) {
        parsed->path = _string_sub(0, p_start - sl, sl);
        param_string = string_create_copy(p_start + 1);
    } else if(!p_start && frag_start) {
        parsed->path = _string_sub(0, frag_start - sl, sl);
    } else {
        // parsed->path = string_create_copy(sl);
    }

    if(p_start) {
        strAL *p_splits = _split(param_string, "&");

        parsed->params = map_create();

        for(size_t i = 0; i < p_splits->size; i++) {
            strAL *pspt = _split_lim(string_array_list_get(p_splits, i), "=",2);

            assert(pspt->size == 2);

            map_put(parsed->params, string_array_list_get(pspt, 0), string_array_list_get(pspt, 1));

            string_arraylist_destroy(&pspt);
        }

        string_arraylist_destroy(&p_splits);
    }

    //free(u);
    return parsed;

}

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


_string_t *_string_create() {

    _string_t *st = malloc(sizeof(_string_t));

    if(st) {

        char *tmp = malloc(1);

        if(tmp) {
            st->len = 0;
            st->size = 1;
            st->ptr = tmp;

            return st;
        }

        /*memory err*/
        return NULL;
    }

    return NULL;
    /*memory err*/
}

_string_t *_string_create_size(size_t size){

    if(size == 0)
        return _string_create();

    _string_t *st = malloc(sizeof(_string_t));

    if(st) {

        char *tmp = malloc(size);

        if(tmp) {
            st->len = 0;
            st->size = size;
            st->ptr = tmp;

            return st;
        }

        /*memory err*/
        return NULL;
    }

    return NULL;
    /*memory err*/
}

void _string_append(_string_t *s, char c) {
    if(s->size-1 == s->len) {
        s->ptr = realloc(s->ptr, s->size + 2);

        if(s->ptr) {
            s->size++;
        }
        /*else malloc() err*/
    }

    s->ptr[s->len] = c;
    s->len++;
    s->ptr[s->len] = '\0';
}


void _string_concat(_string_t *s, char *str) {

    size_t f = strlen(str);
    size_t rem_space = s->size - s->len;

    if(rem_space > f) {

        size_t indx = 0;

        for(size_t i = s->len; indx < f; i++) {
            s->ptr[i] = str[indx];
            indx++;
        }

    } else {

        size_t space_needed = (f - rem_space) + 1;

        s->ptr = realloc(s->ptr, s->size + space_needed);
        s->size += space_needed;

        size_t indx = 0;

        for(size_t i = s->len; indx < f; i++) {
            s->ptr[i] = str[indx];
            indx++;
        }
    }

    s->len += f;
    s->ptr[s->len] = '\0';

}

void _string_expand(_string_t *s, size_t capacity) {
    size_t new_size = s->size + capacity;

    s->ptr = realloc(s->ptr, new_size);

    if(s->ptr) {
        s->size = new_size;
        return;
    }

    /**err*/

}

void _string_destroy(_string_t *s) {
    if(s) {

        if(s->ptr) {

            free(s->ptr);
        }

        free(s);
    }
}


strAL *_split(char *str, char *delim) {
    strAL *splits = string_array_list_create();
    _string_t *x = _string_create();

    size_t delim_len = strlen(delim);

    size_t seq = 0;
    
    for(int i = 0; str[i] != '\0'; i++) {

        if(str[i] == delim[seq]) {
            seq++;
        } else {
            seq = 0;
        }

        _string_append(x, str[i]);

        if(seq == delim_len) {
            x->ptr[x->size - (seq + 1)] = '\0';
            string_array_list_append_allocated(splits, string_create_copy(x->ptr));

            free(x->ptr);

            x->ptr = malloc(1);
            x->ptr[0] = '\0';
            x->len = 0;
            x->size = 1;
        }

    }

    string_array_list_append_allocated(splits, x->ptr);

    free(x);

    return splits;
}


/**
 * @param lim numbers of pieces to projuce.
*/
strAL *_split_lim(char *str, char *delim, int lim) {
    strAL *splits = string_array_list_create();
    _string_t *x = _string_create();

    size_t delim_len = strlen(delim);

    size_t seq = 0;
    
    int num_f = 0;
    for(int i = 0; str[i] != '\0'; i++) {

        if(str[i] == delim[seq]) {
            seq++;
        } else {
            seq = 0;
        }

        _string_append(x, str[i]);

        if(seq == delim_len && num_f < lim - 1) {
            x->ptr[x->size - (seq + 1)] = '\0';
            string_array_list_append_allocated(splits, string_create_copy(x->ptr));

            free(x->ptr);

            x->ptr = malloc(1);
            x->ptr[0] = '\0';
            x->len = 0;
            x->size = 1;

            num_f++;
        }

    }

    string_array_list_append_allocated(splits, x->ptr);

    free(x);

    return splits;
}

char *_string_create_copy(char *str)
{
    int len = strlen(str) + 1;
    char *res = malloc(sizeof(char) * len);
    strcpy(res, str);
    return res;
}

char *_trim(char *buffer)
{
    int start = find_first_not_space(buffer);
    int stop = find_last_not_space(buffer) + 1;

    return _string_sub(start,stop,buffer);
}

size_t _find_last_not_space(char *str)
{
    size_t len = strlen(str);
    size_t i;

    for(i = len-1; i ; i--)
    {
        if(str[i] != ' ')
        {
            if(str[i] != '\t')
                break;
        }
    }

    return i;
}

size_t _find_first_not_space(char *str)
{
    size_t len = strlen(str);
    size_t i;

    for(i = 0; i < len ; i++)
    {
        if(str[i] != ' ')
        {
            if(str[i] != '\t')
                break;
        }
    }

    return i;
}

char *_string_sub(size_t start, size_t stop, char *buffer)
{
    _string_t *temp = _string_create_size(stop - start + 1);

    for (unsigned int i = 0; i < stop; i++)
    {
        if (i >= start)
        {
            _string_append(temp, buffer[i]);
        }

    }

    char *chars = temp->ptr;
    free(temp);
    return chars;
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