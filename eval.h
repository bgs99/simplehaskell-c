#pragma once
#include "dictionary_t.h"
#include "types.h"

bool print_res(const Fun *f);
struct Type *generics_sub(struct Type *t, generics *context);

typedef struct eval_promise{
    dict *glob;
    struct eval_tree *input;
    struct eval_promise *params;
    unsigned paramc;
    struct object *val;
} eval_promise;


object* promise_eval(eval_promise *ep);

void eval_add_arg(eval_tree *tree, eval_tree *arg);

eval_tree *eval_make(Fun *f);

object *eval_expr(dict *glob, const eval_tree *input, eval_promise *params, unsigned parn);


Fun* eval_string(dict *glob, const char *input);
