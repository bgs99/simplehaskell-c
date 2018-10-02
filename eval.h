#pragma once
#include "dictionary_t.h"

bool print_res(const Fun f, char* buff);




void eval_add_arg(eval_tree *tree, eval_tree *arg);

eval_tree* eval_make(const Fun *f);

const Prim* eval_expr(const dict *glob, const eval_tree *input, const Prim **params);


Fun* eval_string(const dict *glob, const char *input);
