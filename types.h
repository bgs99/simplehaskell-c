#pragma once
#include <string.h>
#include <stdbool.h>
#define log(...) fprintf(stderr, __VA_ARGS__)

struct generics;

struct Type{
    union{
        /**
         * @brief Name of the simple function
         */
        const char *name;
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

Parsed parse_t(const char input[]);

size_t print_t(const Type *t, char *s);

bool generic(Type t);
