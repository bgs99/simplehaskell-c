#pragma once
#include "types.h"
typedef unsigned int uint;

typedef enum token_type{
    DATATYPE,
    TYPE_NAME,
    DELIMITER,
    EQUALS,
    IMPORT
} token_type;

typedef struct token{
    token_type  type;
    const char *begin;
    uint length;
} token;

define_list(token, token_list)

char *get_name(token_list **list);
