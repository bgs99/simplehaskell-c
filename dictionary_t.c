#include "dictionary_t.h"
#include <malloc.h>
#include <string.h>
#include "eval.h"
#include "parsing/parser.h"


char s_equal(const char *a, const char *b){
    for(int i = 0; ;i++){
        if(a[i]!=b[i])
            return false;
        if(a[i] == '\0')
            return true;
    }
}

bool pattern_match(const pattern *p, const eval_promise *args, const char *name, const dict *glob){
    if(!p)
        return false;
    if(!s_equal(name, p->t->f->name))
        return false;
    if(!args)
        return true;
    for(unsigned int i = 0; i < p->t->argn; i++){
        if(!p->match[i])
            continue;
        object val = *dict_get(glob, p->match[i]->name)->val;
        object pat = *promise_eval(args[i]);
        if(!object_equal(val, pat)) return false;
        //if(val.i_val != promise_eval(args[i])->i_val) return false;
    }
    return true;
}
const eval_tree* dict_get_eval(const dict *d, const char *name, const eval_promise *args){
    if(!d) return NULL;
    for(const dict* i = d; i; i = i->next){
        for(const pattern_list *j = i->val; j; j = j->next)
            if( pattern_match(j->val, args, name, d))
                return j->val->t;
    }
    return NULL;
}

void dict_add(dict **d, const Fun *value){
    list_add(dict, d, pattern_from_et(eval_make(value)));
}

const Fun* dict_get(const dict *d, const char *name){
    const eval_tree *tree = dict_get_eval(d, name, NULL);
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
    generics *ret = calloc(1, sizeof (generics));
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
            if(i->val) return equal_t(i->val,t,i->val->gen);
            i->val = t;
#ifdef LOGALL
            log("&&Assigned type ");
            log_t(t);
            log(" to type var %s\n", name);
#endif
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
        generics_reset(i->val->val->t->f->type->gen);
}
