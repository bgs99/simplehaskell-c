#pragma once
#include "dictionary_t.h"
#include "type_structs.h"

bool print_res(const struct Fun *f);
struct Type *generics_sub(struct Type *t, struct generics *context);

struct eval_promise{
    dict *glob;
    struct eval_tree *input;
    struct eval_promise *params;
    unsigned paramc;
    struct object *val;
};


struct object* promise_eval(struct eval_promise *ep);

void eval_add_arg(struct eval_tree *tree, struct eval_tree *arg);

struct eval_tree *eval_make(struct Fun *f);

struct object *eval_expr(dict *glob, const struct eval_tree *input, struct eval_promise *params, unsigned parn);


struct Fun* eval_string(dict *glob, const char *input);
