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

void generics_free(generics *d){
    if(!d) return;
    generics_free(d->next);
    free(d);
}

void generics_merge(Type *to,  Type *from){
    if(!from->gen) return;
    generics **dest = to->gen ? &to->gen : malloc(sizeof (generics *));
    for(const generics *i = from->gen; i; i = i->next){
        generics_add(dest, i->key);
    }
    generics_free(from->gen);
}





