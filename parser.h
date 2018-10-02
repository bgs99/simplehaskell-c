#pragma once
#include "hstd.h"
typedef struct parse_res{
  const Type *type;
  const char *left;
  eval_tree *et;
} parse_res;

parse_res parse_app(const dict *local, const dict *glob, const char *input);
parse_res parse_tan(const char *input);
parse_res parse_left(const Fun *f, dict **local, const char *input);
parse_res parse_right(const Type *f, const dict *local, const dict* glob, const char * input);

parse_res parse_fun(const dict *glob, const char *input);

const dict* parse_all(const char *input);
