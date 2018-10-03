#include "dictionary_t.h"
#include <malloc.h>
#include <string.h>
#include "eval.h"


char s_equal(const char *a, const char *b){
    for(int i = 0; ;i++){
        if(a[i]!=b[i])
            return false;
        if(a[i] == '\0')
            return true;
    }
}

void dict_add_eval(const dict **d, const eval_tree *value){
    dict *res = malloc(sizeof (dict));
    res->value = value;
    res->next = *d;
    *d = res;
}
const eval_tree* dict_get_eval(const dict *d, const char *name){
    if(!d) return NULL;
    for(const dict* i = d; i; i = i->next){
        if(s_equal(name, i->value->f->name))
            return i->value;
    }
    return NULL;
}

void dict_add(const dict **d, const Fun *value){
    dict_add_eval(d, eval_make(value));
}

const Fun* dict_get(const dict *d, const char *name){
    const eval_tree *tree = dict_get_eval(d, name);
    return tree ? tree->f : NULL;
}
/**
 * @brief generics_add Adds type variable to a type description. NULL-safe. Should be
 * @param t Destination
 * @param name Type variable's name
 */
void generics_add(Type *t, const char *name){
    for(const generics *i = t->gen; i; i = i->next){
        if(strcmp(i->key, name) == 0)
               return;
    }
    generics *ret = malloc(sizeof (generics));
    ret ->key = name;
    ret->next = t->gen;
    t->gen = ret;
}
/**
 * @brief generics_free Frees memory of generics struct, leaves names. Currently disabled
 * @param d Generics struct
 */
void generics_free(generics *d){
    if(!d) return;
    generics_free(d->next);
    //free(d);
}
/**
 * @brief generics_merge Moves type variables from one type to another. NULL-safe
 * @param to Destination
 * @param from Source
 */
void generics_merge(Type *to,  Type *from){
    if(!from->gen) return;
    for(const generics *i = from->gen; i; i = i->next){
        generics_add(to, i->key);
    }
    generics_free(from->gen);
}

bool generics_bind(generics *g, const char *name, const Type *t){
    for(generics *i = g; i; i = i->next){
        if(strcmp(i->key, name)==0){
            if(i->val) return false;
            i->val = t;
            return true;
        }
    }

    return false;
}

void generics_reset(generics *g){
    for(generics *i = g; i; i = i->next)
        i->val = NULL;
}

void dict_generics_reset(dict *d){
    for(const dict *i = d; i; i = i->next)
        generics_reset(i->value->f->type->gen);
}



