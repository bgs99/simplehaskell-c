#pragma once
#include "types.h"

typedef struct eval_tree{
    const Fun *f; //Function
    unsigned int argn;
    struct eval_tree *arg;//pointer to first arg
    struct eval_tree *next;//pointer to next sibling
} eval_tree;

typedef struct generics{
    const struct generics *next;
    const char *key;
    Type *val;
} generics;

typedef struct dict{
    const struct dict *next;
    const eval_tree *value;
} dict;


void dict_add(const dict **d, const Fun *value);
const Fun* dict_get(const dict *d, const char *name);

void dict_add_eval(const dict **d, const eval_tree *value);
const eval_tree* dict_get_eval(const dict *d,  const char *name);

