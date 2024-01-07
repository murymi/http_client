#include "_string.h"

string_t *string_create()
{
    string_t *t = malloc(sizeof(string_t));
    t->chars = malloc(sizeof(char));
    t->chars[0] = '\0';
    t->size = 0;
    return t;
}


string_t *string_create_from_string(char *string)
{
    int len = strlen(string);
    len++;
    string_t *t = malloc(sizeof(string_t));
    t->chars = malloc(sizeof(char) * len);
    strcpy(t->chars, string);
    t->chars[len] = '\0';

    t->size = len;
    return t;
}

bool string_append(string_t *str, char c)
{
    if (str != NULL)
    {

        int len = str->size;

        str->chars = realloc(str->chars, len + 2);
        str->chars[len] = c;
        str->chars[len + 1] = '\0';

        if(c != '\0')
            str->size++;

        return true;
    }

    return false;
}

bool string_destroy(string_t *str)
{
    if (str != NULL)
    {
        free(str->chars);
        free(str);
        return true;
    }
    return false;
}

bool string_pop(string_t *str)
{
    if (str == NULL)
        return false;
    str->chars[strlen(str->chars) - 1] = '\0';
    str->size--;
    return true;
}
char string_back(string_t *str)
{
    return str->chars[strlen(str->chars) - 1];
}
char string_front(string_t *str)
{
    return str->chars[0];
}

void string_concat(string_t *dest, char *src, size_t len_src)
{
    for (size_t i = 0; i < len_src; i++)
    {
        string_append(dest, src[i]);
        //printf()
    }

    string_append(dest, '\0');
}


char *string_create_copy(char *str)
{
    int len = strlen(str) + 1;
    char *res = malloc(sizeof(char) * len);
    strcpy(res, str);
    return res;
}

char *string_cover(char *str){
    size_t len = strlen(str);
    char *newstr = malloc(sizeof(char) * (len+3));

    newstr[0] = '\'';

    for(size_t i = 1; i < len+1;i++)
    {
        newstr[i] = str[i-1];
    }

    newstr[len+1] = '\'';
    newstr[len+2] = '\0';

    return newstr;
}


bool starts_with_word(char *word, char *str)
{
    bool truth = true;

    if (word == NULL || str == NULL)
        return false;

    if (strlen(word) > strlen(str))
        return false;

    for (size_t i = 0; i < strlen(word); i++)
    {

        //printf("%c%c \n",word[i],str[i]);

        if (word[i] != str[i])
            truth = false;
    }

    return truth;
}

char *string_removechar(char c, char *buffer, size_t len)
{
    string_t *temp = string_create();

    for (size_t i = 0; i < len; i++)
    {
        if (buffer[i] == c)
        {
            continue;
        }

        string_append(temp, buffer[i]);
    }
    string_append(temp, '\0');
    char *chars = temp->chars;
    free(temp);
    return chars;
}

char *string_replacechar(char previous, char current, char *buffer, size_t len)
{
    if(buffer == NULL) return NULL;
    
    string_t *temp = string_create();
    for (size_t i = 0; i < len; i++)
    {
        if (buffer[i] == previous)
        {
            string_append(temp, current);
     
            continue;
        }

        string_append(temp, buffer[i]);
    }
    string_append(temp, '\0');
    char *chars =string_create_copy(temp->chars);
    string_destroy(temp);
    return chars;
}

char *string_removechar_at(unsigned int pos, char *buffer, size_t len)
{
    string_t *temp = string_create();
    for (size_t i = 0; i < len; i++)
    {
        if (i == pos)
        {
            continue;
        }

        string_append(temp, buffer[i]);
    }
    string_append(temp, '\0');
    char *chars = temp->chars;
    free(temp);
    return chars;
}

int string_last_index_of(char c, char *str, unsigned int len)
{
    int pos = -1;
    int i;
    for (i = len - 1; i > -1; i--)
    {
        if (str[i] == c)
        {
            pos = i;
            break;
        };
    }
    return pos;
}

char *string_sub(unsigned int start, unsigned int stop, char *buffer, unsigned int len)
{
    string_t *temp = string_create();
    for (unsigned int i = 0; i < len; i++)
    {
        if (i >= start && i <= stop)
        {
            string_append(temp, buffer[i]);
        }

        continue;
    }
    string_append(temp, '\0');
    char *chars = temp->chars;
    free(temp);
    return chars;
}

bool endswith(char c, char *str)
{
    if (str[strlen(str) - 1] == c)
        return true;
    return false;
}

bool startswith(char c, char *str)
{
    if (str[0] == c)
        return true;
    // putchar('d');
    return false;
}

strAL *split(char delim, char *str, size_t str_size)
{
    strAL *myvec = string_array_list_create();
    string_t *bld = string_create();

    for (size_t i = 0; i < str_size; i++)
    {
        if (str[i] == delim)
        {
            string_append(bld, '\0');
            string_array_list_append(myvec, bld->chars);

            string_destroy(bld);
            bld = string_create();

            continue;
        }

        string_append(bld, str[i]);
    }
    string_append(bld, '\0');
    string_array_list_append(myvec, bld->chars);
    string_destroy(bld);
    return myvec;
}

strAL *split_lim(char delim, char *str, size_t str_size, int lim)
{
    strAL *myvec = string_array_list_create();
    string_t *bld = string_create();

    bool found = false;
    int times_found = 1;

    for (size_t i = 0; i < str_size; i++)
    {

        if ((str[i] == delim && !found) && times_found < lim)
        {
            string_append(bld, '\0');
            string_array_list_append(myvec, bld->chars);
            string_destroy(bld);
            bld = string_create();
            times_found++;
            if (times_found == lim)
                found = true;
            continue;
        }

        string_append(bld, str[i]);
    }

    string_append(bld, '\0');
    string_array_list_append(myvec, bld->chars);
    string_destroy(bld);
    return myvec;
}

char *setchar_at(char *str, char c, size_t pos, size_t len)
{
    if (pos >= len)
        return str;

    string_t *temp = string_create();
    for (size_t i = 0; i < len; i++)
    {
        if (i == pos)
        {
            string_append(temp, c);
            continue;
        }

        string_append(temp, str[i]);
    }
    string_append(temp, '\0');
    char *chars = temp->chars;
    free(temp);
    return chars;
}

char *trim(char *buffer)
{
    int start = find_first_not_space(buffer);
    int stop = find_last_not_space(buffer);

    return string_sub(start,stop,buffer,strlen(buffer));
}

size_t find_last_not_space(char *str)
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

size_t find_first_not_space(char *str)
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


char *string_to_lower(char *str)
{
    string_t *low = string_create();
    size_t len = strlen(str);

    for(size_t i = 0; i < len; i++)
    {
        if(str[i] >= 'A' && str[i] <= 'Z')
        {
            string_append(low,str[i] + 32);
            continue;
        }

        string_append(low,str[i]);
    }

    string_append(low, '\0');
    char *chars = low->chars;
    free(low);
    return chars;
}

char *string_add_char(char *str,char c,size_t pos)
{
    if(str == NULL) return str;

    size_t len = strlen(str);

    if (pos >= len)
        return str;

    string_t *temp = string_create();
    for (size_t i = 0; i < len+1; i++)
    {
        if (i == pos)
        {
            string_append(temp, c);
        }

        string_append(temp, str[i]);
    }
    string_append(temp, '\0');
    char *chars = temp->chars;
    free(temp);
    return chars;
}

strAL *split_v2(char *str, char *delim) {
    strAL *splits = string_array_list_create();
    string_t *x = string_create();

    size_t delim_len = strlen(delim);

    size_t seq = 0;
    
    for(int i = 0; str[i] != '\0'; i++) {

        if(str[i] == delim[seq]) {
            seq++;
        } else {
            seq = 0;
        }

        string_append(x, str[i]);

        if(seq == delim_len) {
            x->chars[x->size - seq] = '\0';
            string_array_list_append_allocated(splits, string_create_copy(x->chars));

            free(x->chars);

            x->chars = malloc(sizeof(char));
            x->chars[0] = '\0';
            x->size = 0;
        }

    }

    string_array_list_append_allocated(splits, x->chars);

    free(x);

    return splits;
}