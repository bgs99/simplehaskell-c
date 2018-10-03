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
    bool simple;
} Type;

struct eval_promise;

typedef union Prim{
    int i_val;
    char c_val;
    double d_val;
    bool b_val;
    const union Prim* (*f_val)(const struct eval_promise*);
} Prim;

typedef struct Fun{
    const char *name;
    const Type *type;
    const Prim *val;
    unsigned int lid;//0 if not parameter, else is parameter id + 1
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

bool generic(Type t);
