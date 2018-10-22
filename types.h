//#define LOGALL

#pragma once
#include <string.h>
#include <stdbool.h>
#define log(...) fprintf(stderr, __VA_ARGS__)
#include <stdio.h>

#define define_list(type, name) \
    typedef struct name{ \
        type *val; \
        struct name *next;\
    } name;
#define list_create(name, val) (name) {val, NULL}
#define list_add(name, dict, value) \
    { \
        name *ret = malloc(sizeof(name)); \
        ret->val = value; \
        ret->next = *dict; \
        *dict = ret; \
    }

const char* alloc_name(const char* name);

struct generics;
struct token_list;
struct fun_list;

struct Type{
    union{
        struct{
            struct fun_list *constructors;
            /**
             * @brief Name of the simple function
             */
            const char *name;
        };
        struct {
            /**
             * @brief Argument of the complex function
             */
            struct Type *arg;
            /**
             * @brief Return type of the complex function
             */
            struct Type *ret;
            /**
             * @brief List of type variables and substitutions.
             * Only useful for complex functions
             */
            struct generics *gen;
        };
    };
    /**
     * @brief If simple, function has only a name, otherwise it has argument and return type
     */
    bool simple;
};
/**
 * @struct Type
 * @brief Type of the function
 */
typedef struct Type Type;

struct eval_promise;

struct object{
    const Type *type;
    /**
     * @brief Index of type's constructor
     */
    const char *name;
    struct eval_promise *args;
};

typedef struct object object;

union Prim{
    const union Prim* (*f_val)(const struct eval_promise*);
    object o_val;
};
/**
 * @union
 * @brief Primitive value/function container
 */
typedef union Prim Prim;

typedef struct Fun{
    const char *name;
    Type *type;
    const Prim *val;
    unsigned int lid;//0 if not parameter, else is parameter id + 1
} Fun;

typedef struct Parsed{
    Type *ret;
    const char *left;
} Parsed;

struct generics{
    /**
     * @brief Next element of the list
     */
    struct generics *next;
    /**
     * @brief Name of generic type
     */
    const char *key;
    /**
     * @brief Actual type, if present
     */
    const Type *val;
};
/**
 * @struct generics
 * @brief This structure represents list of type substitutions
 */
typedef  struct generics generics;



const Type* last_type(const Type *t);

bool equal_t(const Type *a, const Type *b, generics *context);

Type *apply_t(const Type *a, const Type *b);

Parsed parse_t(struct token_list **input);

void fprint_t(const Type *t, FILE *f);
#define print_t(type) fprint_t(type, stdout)
#define log_t(type) fprint_t(type, stderr)

void fprint_context(generics *g, FILE *f);
#define print_context(gen) fprint_context(gen, stdout)
#define log_context(gen) fprint_context(gen, stderr)

bool generic(Type t);

Type* type_make(const char *name);
Type* type_add(Type *fun, Type *arg);
