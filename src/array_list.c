#include "array_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


strAL *string_array_list_create()
{
    strAL *tmp = malloc(sizeof(strAL));

    if (tmp)
    {
        tmp->size = 0;
        tmp->capacity = 0;
        tmp->list = NULL;
        return tmp;
    }

    __assert("Successfull memory allocation\n", __FILE__, __LINE__);
}

void string_array_list_add_one(strAL *sal) {
    char **tmp = realloc(sal->list, sizeof(char *) * (sal->capacity + 1));

    if(tmp){
        sal->list = tmp;
        sal->capacity++;
        return;
    }

    __assert("Successfull memory allocation\n", __FILE__, __LINE__);
}

void string_array_list_append(strAL *al, char *str)
{

    if(al->size >= al->capacity) {
        string_array_list_add_one(al);
    }

    char *cc = malloc(strlen(str) + 1);

    if(cc){
        strcpy(cc, str);
        al->list[al->size] = cc;
        al->size += 1;
        return;
    }
                    
            
    __assert("Successfull memory allocation\n", __FILE__, __LINE__);

}

void string_array_list_append_allocated(strAL *al, char *str)
{

    if(al->size >= al->capacity) {
        string_array_list_add_one(al);
    }

    {
        al->list[al->size] = str;
        al->size += 1;
        return;
    }

}

void string_array_list_print(strAL *a)
{
    for (size_t i = 0; i < a->size; i++)
    {
        puts(a->list[i]);
    }
}

char *string_array_list_get(strAL *a, size_t pos)
{
    if (pos < a->size)
    {
        return a->list[pos];
    }

    __assert("Index in bounds\n", __FILE__, __LINE__);
}

char *string_array_list_get_copy(strAL *a, size_t pos)
{
    if (pos < a->size)
    {
        char *val = a->list[pos];
        char *tmp = malloc(strlen(val) + 1);

        if (tmp)
        {
            strcpy(tmp, val);
            return tmp;
        }

        __assert("Successfull memory allocation\n", __FILE__, __LINE__);
    }

    __assert("Index in bounds\n", __FILE__, __LINE__);
}

char **string_array_list_get_array(strAL *sal, size_t start, size_t end)
{
    if (start < sal->size && end < sal->size)
    {
        char **res = sal->list + start;
        res[end - start] = NULL;

        return res;
    }

    __assert("Index in bounds\n", __FILE__, __LINE__);
}

void string_array_list_add(strAL *sal, char *value, size_t pos)
{
    if (pos <= sal->size)
    {

        if (pos == sal->size)
        {
            string_array_list_append(sal, value);
            return;
        }

        

        if (sal->size >= sal->capacity){
            string_array_list_add_one(sal);
        }

        {

            for (size_t i = sal->size; i >= pos; i--)
            {

                if (i > 0)
                    sal->list[i] = sal->list[i - 1];

                if (i == pos)
                {
                    char *tmp = malloc(strlen(value) + 1);

                    if (tmp)
                    {
                        strcpy(tmp, value);
                        sal->list[i] = tmp;
                        break;
                    }

                    __assert("Successfull memory allocation\n", __FILE__, __LINE__);
                }
            }

            sal->size++;
            return;
        }

        __assert("Successfull memory allocation\n", __FILE__, __LINE__);
    }

    __assert("valid index\n", __FILE__, __LINE__);
}




void string_arraylist_destroy(strAL **sal) {
    for(size_t i = 0; i < (*sal)->size; i++){
        free((*sal)->list[i]);
    }

    free((*sal)->list);

    free(*sal);
    *sal = NULL;
}

void string_array_list_delete(strAL *sal, size_t pos) {

    if(pos < sal->size){

        for(size_t i = pos; i < sal->size; i++){

            if(i == pos) free(sal->list[pos]);

            if(i < (sal->size - 1)) 
                sal->list[i] = sal->list[i+1];
        }

        sal->size--;
        return;
    }

    __assert("valid index\n", __FILE__, __LINE__);
}



strAL *string_array_list_create_capacity(size_t capacity){
    strAL *tmp = string_array_list_create();
    char **xc = malloc(capacity * sizeof(char *));

    if(xc){
        tmp->list = xc;
        tmp->capacity = capacity;
        return tmp;
    }

    __assert("Successfull memory allocation\n", __FILE__, __LINE__);
}

/*
int mainh()
{
    strAL *x = string_array_list_create_capacity(27);
    int size = 1;
    char ***test = NULL;

    for(int i = 0; i < 20; i++){
        if(i % 2)
            string_array_list_add(x, "hello", 0);
        else
            string_array_list_append(x, "world");
        //size++;
        //test= realloc(test, size);
    }

    string_array_list_append(x, "to be delwted");

    for(int i = 0; i < 20; i++){
        printf("size ->%d  capacity ->%ld\n", x->size, x->capacity);
        string_array_list_delete(x,0);
    }

    for(int i = 0; i < 25; i++){
        if(i % 2)
            string_array_list_add(x, "hello", 0);
        else
            string_array_list_append(x, "world");
        //size++;
        //test= realloc(test, size);
    }

    for(int i = 0; i < 25; i++){
        printf("size ->%d  capacity ->%ld\n", x->size, x->capacity);
        string_array_list_delete(x,0);
    }

    //string_array_list_append(x, "how");
    //string_array_list_add(x, "one", 0);
    //string_array_list_add(x, "two", 0);
    ////string_array_list_append(x, "hello world");
    //assert(x->size == 4);
    //string_array_list_add(x, "last", 4);
    ////string_array_list_add(x, "first", 0);

    //string_array_list_delete(x, 1);
    //string_array_list_delete(x,20);

    string_array_list_print(x);
    string_arraylist_destroy(&x);

    assert(x == NULL);
    //string_arraylist_destroy(x);

    // puts(string_array_list_get_copy(x,1));

    // char **arr = string_array_list_get_array(x,0,2);

    // puts(arr[1]);
}
*/