#include "eval.h"
#include "stdio.h"
#include "malloc.h"
#include "parser.h"

bool print_res(const Fun f, char* buff){
    if(!f.type) return false;
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
    tree->argn++;
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

//const Prim* eval_expr(const dict *glob, const eval_tree *input, const Prim *params);

const Prim* collect_args(const dict *glob, const eval_tree *tree, const Prim *params, unsigned int argn){
    Prim *args = malloc(argn * sizeof(Prim));
    int i = 0;
    for(const eval_tree *arg = tree->arg; arg; arg = arg->next){
        args[i++] = *eval_expr(glob, arg, params);
    }
    return args;
}

const Prim* eval_expr(const dict *glob, const eval_tree *input, const Prim *params){
    if(!input) return NULL;
    if(!input->f) return NULL;
    const Fun *f = input->f;

    const Prim *res = f->val;
    const Prim *args = collect_args(glob, input, params, input->argn);
    if(!res){
        if(*f->name == '!'){
            int argi;
            sscanf(f->name+1,"%d",&argi);
            res = &params[argi-1];
        } else {
            const eval_tree *sa =  dict_get_eval(glob, f->name);
            const Prim *ps = collect_args(glob, input, params, sa->argn);
            return eval_expr(glob, sa->arg, ps);
        }
    }
    if(!input->arg){
        return res;
    }

    return res->f_val(args);
}

Fun* eval_string(const dict *glob, const char *input){
    parse_res pr = parse_app(NULL,glob,input);
    Fun *ret = malloc(sizeof (Fun));
    if(!pr.type->simple) return ret;
    const Prim *val = eval_expr(glob, pr.et, NULL);
    ret->type = pr.type;
    ret->val = val;
    return ret;
}
