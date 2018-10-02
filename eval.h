#pragma once
#include "dictionary_t.h"

Fun* apply_v(Fun a, Fun b);

bool print_res(const Fun f, char* buff);

bool apply_f(const Fun a, const Fun b, char* buff);

bool apply(const dict *d, const char * const a, const char * const b, char* buff);



void eval_add_arg(eval_tree *tree, eval_tree *arg);

eval_tree* eval_make(const Fun *f);

Prim eval_expr(dict *glob, const eval_tree *input);


Prim eval_string(dict *glob, const char *input);
