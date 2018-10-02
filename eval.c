#include "eval.h"
#include "stdio.h"
#include "malloc.h"
#include "parser.h"

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

eval_tree* eval_make(const Fun *f){
    eval_tree *ret = malloc(sizeof (eval_tree));
    ret->f = f;
    return ret;
}

const Prim* eval_expr(const dict *glob, const eval_tree *input){
    if(!input) return NULL;
    if(!input->f) return NULL;
    const Fun *f = input->f;

    const Prim *res = f->val;
    if(!res)
        res = eval_expr(glob, dict_get_eval(glob, f->name)->arg);

    if(!input->arg){
        return res;
    }

    const eval_tree *arg = input->arg;

    for(; arg; arg = arg->next){
        res = res->f_val(eval_expr(glob, arg));
    }

    return res;
}

Fun* eval_string(const dict *glob, const char *input){
    parse_res pr = parse_app(NULL,glob,input);
    Fun *ret = malloc(sizeof (Fun));
    const Prim *val = eval_expr(glob, pr.et);
    ret->type = pr.type;
    ret->val = val;
    return ret;
}
