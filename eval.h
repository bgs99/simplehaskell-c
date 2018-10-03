#pragma once
#include "dictionary_t.h"

bool print_res(const Fun f);
const Type* generics_sub(const Type *t, generics *context);
typedef struct eval_promise{
    const dict *glob;
    const struct eval_tree *input;
    const struct eval_promise *params;
} eval_promise;


const Prim* promise_eval(eval_promise ep);

void eval_add_arg(eval_tree *tree, eval_tree *arg);

eval_tree* eval_make(const Fun *f);

const Prim* eval_expr(const dict *glob, const eval_tree *input, const eval_promise *params);


Fun* eval_string(const dict *glob, const char *input);
