#pragma once
#include "types.h"

typedef struct dict{
    const struct dict * next;
    const Fun * value;
} dict;
dict* dict_add(const dict *d, const Fun *value);
const Fun* dict_get(const dict *d, const char *name);
