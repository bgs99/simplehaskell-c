#include "hstd.h"
#include <malloc.h>


Prim inc(Prim a){
    a.i_val++;
    return a;
}

Fun* make_f(const char *name, const char* type, const Prim p){
    Fun *ret = malloc(sizeof (Fun));
    char *tname = malloc(sizeof (char)*20);
    strcpy(tname, name);
    ret->name = tname;
    ret->type = parse_t(type).ret;
    ret->val = malloc(sizeof (Prim));
    *ret->val = p;
    return ret;
}

const dict* init(){
    Fun *z = make_f("z", "Nat", (Prim){.i_val=0}),
        *s = make_f("s", "Nat-Nat", (Prim){.f_val=*inc});

    dict *d = NULL;
    d = dict_add(d, z);
    d = dict_add(d, s);
    return d;
}

