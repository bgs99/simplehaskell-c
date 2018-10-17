#include "hstd.h"
#include "parser.h"
#include <malloc.h>
//#define log(...) fprintf(stderr, __VA_ARGS__)
#define getval(i,type) promise_eval(a[i])->type##_val
#define passval(i) return promise_eval(a[i])
#define retval(val,type) Prim *ret = malloc(sizeof(Prim)); ret->type##_val = val; return ret;
#define deff(name) const Prim* name(const eval_promise *a)


deff(inc){
    retval(getval(0,i)+1,i);
}

deff(dec){
    retval(getval(0,i)-1,i);
}

deff(sum){
    retval(getval(0,i)+getval(1,i),i);
}

deff(fif){
    if(getval(0,b))
        passval(1);
    passval(2);
}

deff(eqi){
    retval(getval(0,i)==getval(1,i),b);
}


Fun* make_f(const char *name, const char* type, const Prim p){
    Fun *ret = malloc(sizeof (Fun));
    char *tname = calloc(20, sizeof (char));
    strcpy(tname, name);
    ret->name = tname;
    const token_list *tl = tokenize(&type);
    ret->type = parse_t(&tl).ret;
    Prim *val = malloc(sizeof (Prim));
    *val = p;
    ret->lid = 0;
    ret->val = val;
    return ret;
}

const dict* init(){
    Fun     *z = make_f("z", "Int", (Prim){.i_val=0}),
            *s = make_f("s", "Int-Int", (Prim){.f_val=&inc}),
            *p = make_f("sum","Int-Int-Int", (Prim){.f_val=&sum}),
            *ip = make_f("if", "Bool-a-a-a",(Prim){.f_val=&fif}),
            *t_f = make_f("True", "Bool", (Prim){.b_val=true}),
            *b_f = make_f("False", "Bool", (Prim){.b_val=false}),
            *eq_f = make_f("equal", "a-a-Bool",(Prim){.f_val=&eqi});

    const dict **d = calloc(1, sizeof (dict*));
    dict_add(d, z);
    dict_add(d, s);
    dict_add(d, p);
    dict_add(d, ip);
    dict_add(d, t_f);
    dict_add(d, b_f);
    dict_add(d, eq_f);
    return *d;
}

