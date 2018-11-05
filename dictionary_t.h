#pragma once
#include "type_structs.h"

define_list(pattern_list, dict)

define_list(struct Type, type_list)

struct Fun *dict_get(const dict *d, struct word name);
struct arg *args_get(const arg_list *d, struct word name);

struct eval_tree* dict_get_eval(const dict *d, struct word name, struct eval_promise *args);
bool is_const(struct word input);
void dict_add(dict **d, struct Fun *value);
void dict_it(dict **d, struct Fun *value);
void args_add(arg_list **d, struct arg *value);
void args_add_self(arg_list **d, struct Fun *value);
void generics_add(struct Type *t, struct word name);

void generics_merge(struct Type *to, struct Type *from);
bool generics_bind(struct generics *g, struct word name, struct Type *t);
void dict_generics_reset(dict *d);
void dict_free(dict *d);
