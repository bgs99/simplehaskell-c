//#define LOGALL

#pragma once
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "list.h"
#include "parsing/parser_common.h"

const char* alloc_name(const char* name);

struct generics;
struct syntax_tree;
struct fun_list;


/**
 * @brief struct Type of the function
 */
struct Type{
    union{
        struct{
            /**
             * @brief Name of the simple function
             */
            struct word name;
        };
        struct {
            /**
             * @brief Argument of the complex function
             */
            struct Type *arg;
            /**
             * @brief Return struct Type of the complex function
             */
            struct Type *ret;

        };
    };
    /**
     * @brief List of struct Type variables and substitutions.
     */
    struct generics *gen;
    /**
     * @brief If simple, function has only a name, otherwise it has argument and return struct Type
     */
    bool simple;
};

struct eval_promise;

struct object{
    int argc;
    struct word name;
    struct eval_promise *args;
};

typedef struct object object;


typedef struct Fun{
    struct word name;
    struct Type *type;
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
     * @brief Name of generic struct Type
     */
    struct word key;
    /**
     * @brief Actual struct Type, if present
     */
    struct Type *val;
};
/**
 * @struct generics
 * @brief This structure represents list of struct Type substitutions
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
    int argn;
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


struct Type* last_type(struct Type *t);

bool equal_t(struct Type *a, struct Type *b, generics *context);

struct Type *apply_t(const struct Type *a, struct Type *b);

void fprint_t(const struct Type *t, FILE *f);
#define print_t(type) fprint_t(type, stdout)
#define log_t(type) fprint_t(type, stderr)

void fprint_context(generics *g, FILE *f);
#define print_context(gen) fprint_context(gen, stdout)
#define log_context(gen) fprint_context(gen, stderr)

bool generic(struct Type t);

struct Type* type_make(struct word name);
struct Type* type_add(struct Type *fun, struct Type *arg);

bool object_equal(object a, object b);
void reset_generics(struct Type *t);

bool name_equal(struct word a, struct word b);
