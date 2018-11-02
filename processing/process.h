#pragma once
#include "../types.h"
#include "../dictionary_t.h"

struct fun_def{
  const Type *type;
  pattern_list *et;
};

struct fun_def process_app(const dict *loc, const dict *glob, struct syntax_tree input);

pattern_list* pattern_from_et(eval_tree *et);
