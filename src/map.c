#include "map.h"

bstree *bstree_create(char *key, char *value){
    bstree *tmp = malloc(sizeof(bstree));
    if(tmp){

        char *_key = malloc(strlen(key) + 1);
        if(_key){
            strcpy(_key, key);
        } 
        else
            __assert("Successfull memory allocation\n", __FILE__, __LINE__);

        char *_value = malloc(strlen(value) + 1);

        if(_value){
            strcpy(_value, value);
        }
        else
            __assert("Successfull memory allocation\n", __FILE__, __LINE__);

        tmp->key = _key;
        tmp->value = _value;

        tmp->right = tmp->left = NULL;

        return tmp;
    }

    __assert("Successfull memory allocation\n", __FILE__, __LINE__);
    return NULL;
}

map_t *map_create() {
    map_t *tmpmap = malloc(sizeof(map_t));

    if(tmpmap){
        tmpmap->tree = NULL;
        tmpmap->size = 0;
        return tmpmap;
    }

    __assert("Successfull memory allocation\n", __FILE__, __LINE__);
    return NULL;
}

void bstree_insert(bstree **b,char *key, char *value){

    if(*b == NULL){
        *b = bstree_create(key, value);
        return;
    }

    if(strcmp((*b)->key, key) > 0){
        bstree_insert(&((*b)->right), key, value);
    }else if(strcmp((*b)->key, key) < 0){
        bstree_insert(&((*b)->left), key, value);
    }
    else{

    //duplicate key
    }
}

void map_put(map_t *map, char *key, char *value) {
    if(map->tree == NULL && map->size == 0){
        map->tree = bstree_create(key, value);
        map->size++;
        return;
    }

    bstree_insert(&(map->tree), key, value);
    map->size++;
}


void deleteTree(bstree **b){
    if(*b){
        deleteTree(&((*b)->left));
        deleteTree(&((*b)->right));

        if((*b)->key) free((*b)->key);
        if((*b)->value) free((*b)->value);
        free(*b);
        *b = NULL;
    }
}

void map_destroy(map_t **m) {
    if(*m){
        if((*m)->tree){
            deleteTree(&(*m)->tree);
        }

        free(*m);
        *m = NULL;
    }
}

char *bstree_get(bstree *b, char *key) {
    if(b){
        if(strcmp(key, b->key) < 0){
            return bstree_get(b->right, key);
        } else if(strcmp(key, b->key) > 0){
            return bstree_get(b->left, key);
        } else {
            return b->value;
        }
    } else {
        return NULL;
    }
}

char *map_get(map_t *m, char *key) {
    if(m){
        return bstree_get(m->tree, key);
    }

    return NULL;
}

bstree *minVal(bstree *b){

    bstree *current = b;

    while(current && (current->left))
        current = current->left;

    return current;
}

void deleteNode(bstree **b, char *key){
    if(b == NULL)
        return;

    if(strcmp(key, (*b)->key) > 0){
        deleteNode(&((*b)->left), key);
    }
    else if(strcmp(key, (*b)->key) < 0){

        deleteNode(&((*b)->right), key);

    } else {
        if(!((*b)->left)){
            bstree *tmp = (*b)->right;

            free((*b)->key);
            free((*b)->value);

            free(*b);
            *b = tmp;
            return;
        }

        if(!((*b)->right)){
            bstree *tmp = (*b)->left;

            free((*b)->key);
            free((*b)->value);

            free(*b);
            *b = tmp;
            return;
        }

        bstree *min = minVal(*b);

        (*b)->key = min->key;

        deleteNode(&((*b)->right), min->key);
    }
}

void map_delete(map_t *m, char *key) {
    if(m) {
        deleteNode(&(m->tree),key);
        m->size--;
    }
}

inline void in_order(bstree *b){
    if(b){
        in_order(b->left);
        printf("%s> %s\n", b->key, b->value);
        in_order(b->right);
    }
}

void do_fn(bstree *b, transform_func fn){
    if(b){
        do_fn(b->left, fn);
        fn(b->key, b->value);
        do_fn(b->right, fn);
    }
}

void do_fn_wret(bstree *b, map_func fn, string_t *str){
    if(b){
        do_fn_wret(b->left, fn, str);
        fn(b->key, b->value, str);
        do_fn_wret(b->right, fn, str);
    }
}

inline void map_print(map_t *map) {
    if(map){
        in_order(map->tree);
    }
}


void for_each(map_t *m, transform_func fn){
    do_fn(m->tree, fn);
}

char *for_each_map(map_t *m, map_func fn){
    string_t *res = string_create();

    do_fn_wret(m->tree, fn, res);

    char *tmp = res->chars;
    free(res);
    return tmp;
}

int mainy() {

    map_t *m = map_create();

    map_put(m, "hello", "world");
    map_put(m, "green", "world");
    map_put(m, "greenish", "world");
    map_put(m, "jack", "grealish");
    map_put(m, "andy", "carrol");

    //map_delete(m, "andy");

    //assert(m->size == 4);

    //map_print(m);

    char *x = map_get(m, "jack");

    if(x){
        printf("val -> %s", x);
    } else {
        puts("NULL");
    }

    map_destroy(&m);

    assert(m == NULL);

    return 0;
}