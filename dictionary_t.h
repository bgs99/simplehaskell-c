#pragma once
#include "types.h"

typedef struct pattern_list pattern_list;
struct eval_promise;

struct eval_tree{
    /**
     * @brief A function to be called
     */
    const Fun *f; //Function
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


define_list(pattern_list, dict)

/**
 * @brief Gets function from a dictionary
 * @param d Dictionary to search
 * @param name Name of the function to find
 */
const Fun* dict_get(const dict *d, const char *name);
/**
 * @brief Gets evaluation tree from a dictionary
 * @param d Dictionary to search
 * @param name Name of the function to find
 */
const eval_tree* dict_get_eval(const dict *d,  const char *name, const struct eval_promise *args);

void dict_add(dict **d, const Fun *value);
void generics_add(Type *t, const char *name);

void generics_merge(Type *to, Type *from);
bool generics_bind(generics *g, const char *name, const Type *t);
void dict_generics_reset(dict *d);
