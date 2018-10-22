#pragma once
#include "hstd.h"
typedef struct pattern{
    eval_tree *t;
    struct pattern *next;
    const Fun **match;
} pattern;

typedef struct parse_res{
  const Type *type;
  pattern *et;
} parse_res;

typedef struct token_list{
    const char *begin;
    unsigned long len;
    struct token_list *next;
} token_list;
constructor_list* parse_datatype(const token_list **input);
token_list* tokenize(const char **input);
const char* get_name(const token_list **tl);
parse_res parse_app(const pattern_list *local, const pattern_list *glob, const token_list **input);
parse_res parse_tan(const token_list **input);
void parse_left(const Fun *f, pattern_list **local, const token_list **input);
parse_res parse_right(const Type *f, const pattern_list *local, const pattern_list* glob, const token_list **input);

parse_res parse_fun(const pattern_list *glob, const char **input, const token_list **left);

pattern_list *parse_all(const char *input);

pattern* pattern_from_et(eval_tree *et);
