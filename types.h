#pragma once
#include <string.h>
#include <stdbool.h>

typedef struct Type{
    union{
        const char *name;
        struct {
            const struct Type *arg;
            const struct Type *ret;
        } func;
    } val;
    int simple;
} Type;
typedef union Prim{
    int i_val;
    char c_val;
    double d_val;
    bool b_val;
    union Prim (*f_val)(union Prim);
} Prim;

typedef struct Fun{
    const char *name;
    const Type *type;
    const char *def;
} Fun;


typedef struct Parsed{
    Type *ret;
    const char *left;
} Parsed;

const Type* last_type(const Type *t);

bool equal_t(Type a, Type b);

const Type* apply_t(Type a, Type b);

Parsed parse_t(const char input[]);

size_t print_t(const Type *t, char *s);
