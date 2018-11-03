#pragma once
#include "dictionary_t.h"
#include "types.h"

bool print_res(const Fun *f);
const Type* generics_sub(const Type *t, generics *context);

typedef struct eval_promise{
    const dict *glob;
    const struct eval_tree *input;
    struct eval_promise *params;
    struct object *val;
} eval_promise;


object* promise_eval(eval_promise *ep);

void eval_add_arg(eval_tree *tree, eval_tree *arg);

eval_tree* eval_make(Fun *f);

object *eval_expr(const dict *glob, const eval_tree *input, eval_promise *params);


Fun* eval_string(const dict *glob, const char *input);
