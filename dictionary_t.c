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


///At least one value
const Fun* dict_get(const dict *d, const char* name){
    const eval_tree *tree = dict_get_eval(d, name);
    return tree ? tree->f : NULL;
}

