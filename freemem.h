#pragma once
#include "list.h"
#include "dictionary_t.h"




void mark_ptr(void *ptr);

void promise_free(struct eval_promise *p);
void generics_free(generics *g);
void object_free(object *o);
void type_free(Type *t);
void function_free(Fun *f);
void arg_free(struct arg *a);
void eval_tree_free(eval_tree *t);
void arg_list_free(arg_list *al);
void pattern_free(pattern *p);
void pattern_list_free(pattern_list *pl);
void dict_free(dict *d);

void free_all(dict *d);
void generics_free(generics *g);
void syntax_tree_free(struct syntax_tree tree);
