#pragma once
#include <stdbool.h>
#include "../list.h"

struct word{
    const char *begin;
    long length;
};
enum token_type{
    DATATYPE,
    CONSTRUCTOR_LIST,
    CONSTRUCTOR,
    NAME,
    TYPE_LIST,
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
