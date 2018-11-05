#include "dictionary_t.h"
#include <malloc.h>
#include <string.h>
#include "eval.h"
#include "parsing/parser.h"
#include "processing/process.h"
#include "freemem.h"

/**
 * @brief Checks if name corresponds to a constant
 * @param input name
 * @return true if constant, false otherwise
 */
bool is_const(struct word input){
    char c = *input.begin;
    return (c >= '0' && c <='9') || (c >= 'A' && c <= 'Z');
}

/**
 * @brief Checks if two stricgs are equal (yes, it's redundant)
 * @param a first string
 * @param b second string
 * @return true if equal, false otherwise
 */
bool s_equal(const char *a, const char *b){
    for(int i = 0; ;i++){
        if(a[i]!=b[i])
            return false;
        if(a[i] == '\0')
            return true;
    }
}

bool match_arg(const struct arg *pat, struct eval_promise *arg, const dict *glob){
    if(!is_const(pat->match->name))
        return true;
    struct object *par = promise_eval(arg);
    if(!par)
        return false;
    if(!name_equal(pat->match->name, par->name))
        return false;
    int n = 0;
    for(arg_list *i = pat->args; i; i = i->next, n++){
        if(!match_arg(i->val, par->args + n, glob)){
            return false;
        }
    }
    return true;
}
/**
 * @brief Checks if arguments correspond to a pattern
 * @param p pattern
 * @param args arguments
 * @param glob global dictionary of names
 * @return true if they correspond, false otherwise
 */
bool pattern_match(const struct pattern *p, struct eval_promise *args, const dict *glob){
    if(!p)
        return false;
    if(!args)
        return true;
    int n = 0;
    for(arg_list *i = p->args->next; i; i = i->next, n++){
        if(*i->val->match->name.begin == '_')
            continue;
        if(!match_arg(i->val, args + n, glob)){
            return false;
        }
    }
    return true;
}
/**
 * @brief Gets evaluation tree that correspond to a function and an array of arguments
 * @param d dictionary of names
 * @param name name of the function
 * @param args array of arguments
 * @return evaluation tree on success, NULL on fail
 */
struct eval_tree *dict_get_eval(const dict *d, struct word name, struct eval_promise *args){
    if(!d) return NULL;
    for(const dict* i = d; i; i = i->next){
        if(!name_equal(name, i->val->val->t->f->name))
            continue;
        for(const pattern_list *j = i->val; j; j = j->next)
            if( pattern_match(j->val, args, d))
                return j->val->t;
    }
    return NULL;
}
/**
 * @brief Adds evaluation tree from a function to the dictionary
 * @param d dictionary of names
 * @param value function
 */
void dict_add(dict **d, struct Fun *value){
    if(!value)
        return;
    pattern_list *p = pattern_from_et(eval_make(value));
    if(!p)
        return;
    list_add(dict, d, p);
}

void dict_it(dict **d, struct Fun *value){
    pattern_list *p = pattern_from_et(eval_make(value));
    if(name_equal((*d)->val->val->t->f->name, value->name))
        (*d)->val = p;
    else list_add(dict, d, p);
}

void args_add(arg_list **d, struct arg *value){
    list_add_last(arg_list, d, value);
}
void args_add_self(arg_list **d, struct Fun *value){
    struct arg *self = malloc(sizeof (struct arg));
    self->args = NULL;
    self->complex = false;
    self->match = value;
    list_add(arg_list, d, self);
}
/**
 * @brief Gets function from dictionary
 * @param d dictionary of names
 * @param name name of the function
 * @return function on success, NULL on fail
 */
struct Fun* dict_get(const dict *d, struct word name){
    const struct eval_tree *tree = dict_get_eval(d, name, NULL);
    return tree ? tree->f : NULL;
}
struct arg* args_get(const arg_list *d, struct word name){
    for(const arg_list *i = d; i; i = i->next){
        if((!i->val->args) && (name_equal(i->val->match->name, name)))
            return i->val;
        struct arg *rec = args_get(i->val->args, name);
        if(rec)
            return rec;
    }
    return NULL;
}
/**
 * @brief Adds struct Type variable to a struct Type description
 * @param t Destination
 * @param name struct Type variable's name
 */
void generics_add(struct Type *t, struct word name){
    for(const struct generics *i = t->gen; i; i = i->next){
        if(name_equal(i->key, name))
               return;
    }
    struct generics *ret = calloc(1, sizeof (struct generics));
    ret->key = name;
    ret->next = t->gen;
    t->gen = ret;
    mark_ptr(ret);
}
/**
 * @brief generics_merge Moves struct Type variables from one struct Type to another. NULL-safe
 * @param to Destination
 * @param from Source
 */
void generics_merge(struct Type *to,  struct Type *from){
    if(!from->gen) return;
    for(const struct generics *i = from->gen; i; i = i->next){
        generics_add(to, i->key);
    }
    //generics_free(from->gen);
}
/**
 * @brief Binds generic argument to a struct Type
 * @param g generics context
 * @param name name of the argument
 * @param t struct Type
 * @return true if argument is generic and can be bound, false otherwise
 */
bool generics_bind(struct generics *g, struct word name, struct Type *t){
    for(struct generics *i = g; i; i = i->next){
        if(name_equal(i->key, name)){
            if(i->val)
                return equal_t(i->val,t,i->val->gen);
            i->val = t;
#ifdef LOGALL
            fprintf(stderr, "&&Assigned struct Type ");
            log_t(t);
            fprintf(stderr, " to struct Type var %s\n", name);
#endif
            return true;
        }
    }

    return false;
}
/**
 * @brief Removes bindings from a context
 * @param g generic context
 */
void generics_reset(struct generics *g){
    for(struct generics *i = g; i; i = i->next)
        i->val = NULL;
}
/**
 * @brief Removes bindings from contexts in whole dictionary
 * @param d Dictionary of names
 */
void dict_generics_reset(dict *d){
    for(const dict *i = d; i; i = i->next)
        generics_reset(i->val->val->t->f->type->gen);
}

