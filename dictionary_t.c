#include "dictionary_t.h"
#include <malloc.h>
#include <string.h>

dict* dict_add(const dict *d, const Fun *value){
    dict *res = malloc(sizeof (dict));
    res->next = d;
    res->value = value;
    return res;
}

char s_equal(const char *a, const char *b){
    for(int i = 0; ;i++){
        if(a[i]!=b[i])
            return false;
        if(a[i] == '\0')
            return true;
    }
}

///At least one value
const Fun* dict_get(const dict *d, const char* name){
    for(const dict* i = d; i; i = i->next){
        if(s_equal(name, i->value->name))
            return i->value;
    }
    return NULL;
}
