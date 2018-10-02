#include "eval.h"
#include "stdio.h"
#include "malloc.h"

Fun* apply_v(const Fun a, const Fun b){
    Fun* res = malloc(sizeof (Fun));
    res->name = NULL;
    *res->val = a.val->f_val(*b.val);
    res->type = apply_t(*a.type, *b.type);
    return res;
}

bool print_res(const Fun f, char* buff){
    if(!f.type->simple) return false;
    if(!strcmp(f.type->val.name, "Nat")){
        sprintf(buff, "%d", f.val->i_val);
    } else if(!strcmp(f.type->val.name, "Double")){
        sprintf(buff, "%f", f.val->d_val);
    } else if(!strcmp(f.type->val.name, "Bool")){
        if(f.val->b_val)
            sprintf(buff, "true");
        else sprintf(buff, "false");
    } else if(!strcmp(f.type->val.name, "Char")){
        sprintf(buff, "%c", f.val->c_val);
    }
    return true;
}

bool apply_f(const Fun a, const Fun b, char* buff){
    Fun *res = apply_v(a, b);
    if(!res->type){
        return false;
    }
    buff+=print_t(res->type, buff);
    *buff = '\n';
    buff++;
    if(!print_res(*res,buff)){
        sprintf(buff, "Cannot print a function");
    }
    return true;
}

bool apply(const dict *d, const char * const a, const char * const b, char* buff){
    const Fun *fa = dict_get(d, a);
    const Fun *fb = dict_get(d, b);
    if(!fa || !fb){
        return false;
    }
    return apply_f(*fa,*fb, buff);
}

void eval_add_arg(eval_tree *tree, eval_tree *arg){
    if(!tree){
        return;
    }
    if(!tree->arg){
        tree->arg = arg;
        return;
    }
    eval_tree *t = tree->arg;//get first arg
    for(; t; t = t->next);//skip current args
    //now t has last arg
    t->next = arg;
}

eval_tree* eval_make(Fun *f){
    eval_tree *ret = malloc(sizeof (eval_tree));
    ret->f = f;
    return ret;
}

