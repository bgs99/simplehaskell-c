#pragma once
#include <stdbool.h>
#include "../list.h"
#include <stddef.h>

struct word{
    const char *begin;
    unsigned long length;
};
enum token_type{
    DATATYPE,
    FUNCTION,
    PROGRAM,
    CONSTRUCTOR,
    VALUE_TYPE,
    FUN_TYPE,
    COMPLEX_TYPE,
    IMPORT,
    ANNOTATION,
    DEFINITION,
    ARG_LIST,
    VAR,
    VAL,
    COMPLEX_ARG,
    EXPRESSION,
    UNDEFINED
};

define_list(struct syntax_tree, tree_args)

struct syntax_tree{
    enum token_type type;
    struct word val;
    tree_args *args;
};

extern struct syntax_tree undefined;
struct word read_word(const char **input);
bool accept(const char **input, char c);
bool accept_word(const char **input, const char *word);
void skip_ws(const char **input);
void free_tree_args(tree_args *args);
