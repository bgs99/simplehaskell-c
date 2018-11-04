#pragma once
#include "types.h"

define_list(pattern_list, dict)

define_list(Type, type_list)

Fun *dict_get(const dict *d, struct word name);
struct arg *args_get(const arg_list *d, struct word name);

eval_tree* dict_get_eval(const dict *d, struct word name, struct eval_promise *args);
bool is_const(struct word input);
void dict_add(dict **d, Fun *value);
void args_add(arg_list **d, struct arg *value);
void args_add_self(arg_list **d, Fun *value);
void generics_add(Type *t, struct word name);

void generics_merge(Type *to, Type *from);
bool generics_bind(generics *g, struct word name, Type *t);
void dict_generics_reset(dict *d);
void dict_free(dict *d);
