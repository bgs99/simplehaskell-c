#pragma once
#include "dictionary_t.h"

Fun* apply_v(Fun a, Fun b);

bool print_res(const Fun f, char* buff);

bool apply_f(const Fun a, const Fun b, char* buff);

bool apply(const dict *d, const char * const a, const char * const b, char* buff);

typedef struct eval_tree{
    Fun *f; //Function
    int argn;//counted from 1;
    struct eval_tree *arg;//pointer to first arg
    struct eval_tree *next;//pointer to next sibling
} eval_tree;

void eval_add_arg(eval_tree *tree, eval_tree *arg);

eval_tree* eval_make(Fun *f);
