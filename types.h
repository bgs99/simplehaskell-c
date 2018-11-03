//#define LOGALL

#pragma once
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "list.h"

const char* alloc_name(const char* name);

struct generics;
struct syntax_tree;
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
    int argc;
    const char *name;
    struct eval_promise *args;
};

typedef struct object object;


typedef struct Fun{
    const char *name;
    Type *type;
    object *val;
    unsigned id_depth;
    unsigned int *ids;//NULL if not parameter
} Fun;

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

struct eval_tree{
    /**
     * @brief A function to be called
     */
    Fun *f; //Function
    /**
     * @brief Number of arguments
     */
    unsigned int argn;
    /**
     * @brief Pointer to first arg
     */
    struct eval_tree *arg;
    /**
     * @brief Pointer to next sibling
     */
    struct eval_tree *next;
};
/**
 * @struct eval_tree
 * @brief This structure represents evaluation tree
 */
typedef struct eval_tree eval_tree;

struct arg;
define_list(struct arg, arg_list)

struct arg{
    bool complex;
    Fun *match;
    arg_list *args;
};

typedef struct pattern{
    eval_tree *t;
    arg_list *args;
} pattern;

define_list(pattern, pattern_list)


const Type* last_type(const Type *t);

bool equal_t(const Type *a, const Type *b, generics *context);

Type *apply_t(const Type *a, const Type *b);

void fprint_t(const Type *t, FILE *f);
#define print_t(type) fprint_t(type, stdout)
#define log_t(type) fprint_t(type, stderr)

void fprint_context(generics *g, FILE *f);
#define print_context(gen) fprint_context(gen, stdout)
#define log_context(gen) fprint_context(gen, stderr)

bool generic(Type t);

Type* type_make(const char *name);
Type* type_add(Type *fun, Type *arg);

bool object_equal(object a, object b);
void reset_generics(Type *t);
