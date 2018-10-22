#pragma once
#include "eval.h"
typedef struct pattern{
    eval_tree *t;
    const Fun **match;
} pattern;

define_list(pattern, pattern_list)

typedef struct parse_res{
  const Type *type;
  pattern_list *et;
} parse_res;

typedef struct token{
    const char *begin;
    unsigned long len;
} token;
define_list(token, token_list)
void parse_datatype(Type *name, token_list **input, dict **glob);
token_list* tokenize(const char **input);
const char* get_name(token_list **tl);
parse_res parse_app(const dict *local, const dict *glob, token_list **input);
parse_res parse_tan(token_list **input);
void parse_left(const Fun *f, dict **local, token_list **input);
parse_res parse_right(const Type *f, const dict *local, const dict* glob, token_list **input);

parse_res parse_fun(const dict *glob, const char **input, token_list **left);

dict *parse_all(const char *input);

pattern_list* pattern_from_et(eval_tree *et);
