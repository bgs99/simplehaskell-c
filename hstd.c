#include "hstd.h"
#include <malloc.h>


Prim* inc(const Prim *a){
    Prim *ret = malloc(sizeof (Prim));
    ret->i_val = a[0].i_val+1;
    return ret;
}

Prim* sum(const Prim *a){
    Prim *ret = malloc(sizeof (Prim));
    ret->i_val = a[0].i_val + a[1].i_val;
    return ret;
}


Fun* make_f(const char *name, const char* type, const Prim p){
    Fun *ret = malloc(sizeof (Fun));
    char *tname = malloc(sizeof (char)*20);
    strcpy(tname, name);
    ret->name = tname;
    ret->type = parse_t(type).ret;
    Prim *val = malloc(sizeof (Prim));
    *val = p;
    ret->val = val;
    return ret;
}

const dict* init(){
    Fun *z = make_f("z", "Nat", (Prim){.i_val=0}),
        *s = make_f("s", "Nat-Nat", (Prim){.f_val=&inc}),
        *p = make_f("p","Nat-Nat-Nat", (Prim){.f_val=&sum});

    dict *d = NULL;
    d = dict_add(d, z);
    d = dict_add(d, s);
    d = dict_add(d, p);
    return d;
}

