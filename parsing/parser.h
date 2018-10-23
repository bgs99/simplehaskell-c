#pragma once
#include "dictionary_t.h"
#include "tokens.h"

typedef struct parse_res{
  Type *type;
  pattern_list *et;
} parse_res;

void parse_datatype(Type *name, token_list **input, dict **glob);
token_list* tokenize(const char **input);
parse_res parse_app(const dict *local, const dict *glob, token_list **input);
parse_res parse_tan(token_list **input);
void parse_left(const Fun *f, dict **local, token_list **input);
parse_res parse_right(const Type *f, const dict *local, const dict* glob, token_list **input);

parse_res parse_fun(const dict *glob, const char **input, token_list **left);

dict *parse_all(const char *input);

pattern_list* pattern_from_et(eval_tree *et);
