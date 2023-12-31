#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "_string.h"

typedef struct bstree bstree;
struct bstree {
    char *key;
    char *value;
    bstree *left, *right;
};

typedef struct map_t map_t;
struct map_t {
    size_t size;
    bstree *tree;
};


typedef void (*transform_func)(char *key, char *value);

typedef void (*map_func)(char *key, char *value, string_t *s);

void do_fn(bstree *b, transform_func fn);

void for_each(map_t *m, transform_func fn);

char *for_each_map(map_t *m, map_func fn);

void do_fn_wret(bstree *b, map_func fn, string_t *s);

bstree *bstree_create(char *key, char *value);

map_t *map_create();

void bstree_insert(bstree **b,char *key, char *value);

void map_put(map_t *map, char *key, char *value);


void in_order(bstree *b);

void map_print(map_t *map);

void deleteTree(bstree **b);

void map_destroy(map_t **m);

char *bstree_get(bstree *b, char *key);

char *map_get(map_t *m, char *key);

bstree *minVal(bstree *b);

void deleteNode(bstree **b, char *key);

void map_delete(map_t *m, char *key);