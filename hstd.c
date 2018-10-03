#include "hstd.h"
#include <malloc.h>


const Prim* inc(const eval_promise *a){
    Prim *ret = malloc(sizeof (Prim));
    ret->i_val = promise_eval(a[0])->i_val+1;
    return ret;
}

const Prim* sum(const eval_promise * a){
    Prim *ret = malloc(sizeof (Prim));
    ret->i_val = promise_eval(a[0])->i_val + promise_eval(a[1])->i_val;
    return ret;
}

const Prim* fif(const eval_promise *a){
    if(promise_eval(a[0])->b_val)
        return promise_eval(a[1]);
    return promise_eval(a[2]);
}

const Prim* eqi(const eval_promise *a){
    Prim *ret = malloc(sizeof (Prim));
    ret->b_val = promise_eval(a[0])->i_val == promise_eval(a[1])->i_val;
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
    Fun     *z = make_f("z", "Nat", (Prim){.i_val=0}),
            *s = make_f("s", "Nat-Nat", (Prim){.f_val=&inc}),
            *p = make_f("plus","Nat-Nat-Nat", (Prim){.f_val=&sum}),
            *ip = make_f("if", "Bool-Nat-Nat-Nat",(Prim){.f_val=&fif}),
            *t_f = make_f("true", "Bool", (Prim){.b_val=true}),
            *b_f = make_f("false", "Bool", (Prim){.b_val=false}),
            *eq_f = make_f("equal", "Nat-Nat-Bool",(Prim){.f_val=&eqi});

    const dict **d = malloc(sizeof (dict*));
    dict_add(d, z);
    dict_add(d, s);
    dict_add(d, p);
    dict_add(d, ip);
    dict_add(d, t_f);
    dict_add(d, b_f);
    dict_add(d, eq_f);
    return *d;
}

