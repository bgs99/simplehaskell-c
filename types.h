//#define LOGALL

#pragma once
#include <string.h>
#include <stdbool.h>
#define log(...) fprintf(stderr, __VA_ARGS__)
#include <stdio.h>

const char* alloc_name(const char* name);

struct generics;
struct token_list;
struct constructor_list;

struct Type{
    union{

        struct{
            struct constructor_list *constructors;
            /**
             * @brief Name of the simple function
             */
            const char *name;
        };
        struct {
            /**
             * @brief Argument of the complex function
             */
            const struct Type *arg;
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
    Type *type;
    /**
     * @brief Index of type's constructor
     */
    int c_id;
    struct eval_promise *args;
};

typedef struct object object;

struct constructor_list{
    struct constructor_list *next;
    char *name;
    int argc;
    Type **arg_types;
};

typedef struct constructor_list constructor_list;

union Prim{
    /**
     * @brief Integer value
     */
    int i_val;
    /**
     * @brief Char value
     */
    char c_val;
    /**
     * @brief Double precision float value
     */
    double d_val;
    /**
     * @brief Boolean value
     */
    bool b_val;
    /**
     * @brief Function value
     */
    const union Prim* (*f_val)(const struct eval_promise*);
};
/**
 * @union
 * @brief Primitive value/function container
 */
typedef union Prim Prim;

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

const Type* apply_t(const Type *a, const Type *b);

Parsed parse_t(const struct token_list **input);

void fprint_t(const Type *t, FILE *f);
#define print_t(type) fprint_t(type, stdout)
#define log_t(type) fprint_t(type, stderr)

void fprint_context(generics *g, FILE *f);
#define print_context(gen) fprint_context(gen, stdout)
#define log_context(gen) fprint_context(gen, stderr)

bool generic(Type t);

Type* type_make(const char *name);
Type* type_add(Type *fun, Type *arg);
