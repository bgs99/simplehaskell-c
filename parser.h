#pragma once
#include "hstd.h"
typedef struct parse_res{
  const Type *type;
  eval_tree *et;
} parse_res;

parse_res parse_app(const dict *local, const dict *glob, const char **input);
parse_res parse_tan(const char **input);
void parse_left(const Fun *f, const dict **local, const char **input);
parse_res parse_right(const Type *f, const dict *local, const dict* glob, const char **input);

parse_res parse_fun(const dict *glob, const char **input);

const dict* parse_all(const char *input);
