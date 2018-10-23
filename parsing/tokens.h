#pragma once
#include "types.h"
typedef unsigned int uint;

typedef enum token_type{
    DATATYPE,
    TYPE_NAME,
    FUN_NAME,
    DELIMITER,
    IMPORT,
    POPEN,
    PCLOSE,
    UNDEFINED
} token_type;

typedef struct token{
    token_type  type;
    const char *begin;
    uint length;
} token;

define_list(token, token_list)

char *get_name(token_list **list);

token_list* tokenize(const char **input);
