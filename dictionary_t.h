#pragma once
#include "types.h"

define_list(pattern_list, dict)

define_list(Type, type_list)

Fun *dict_get(const dict *d, const char *name);
struct arg *args_get(const arg_list *d, const char *name);

eval_tree* dict_get_eval(const dict *d,  const char *name, const struct eval_promise *args);
bool is_const(const char *input);
void dict_add(dict **d, Fun *value);
void args_add(arg_list **d, struct arg *value);
void args_add_self(arg_list **d, Fun *value);
void generics_add(Type *t, const char *name);

void generics_merge(Type *to, Type *from);
bool generics_bind(generics *g, const char *name, const Type *t);
void dict_generics_reset(dict *d);
