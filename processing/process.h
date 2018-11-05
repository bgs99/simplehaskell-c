#pragma once
#include "../type_structs.h"
#include "../dictionary_t.h"

struct fun_def{
  struct Type *type;
  pattern_list *et;
};

struct fun_def process_app(const arg_list *loc, const dict *glob, struct syntax_tree input);
dict *process_all(const char *input);
pattern_list *pattern_from_et(struct eval_tree *et);
