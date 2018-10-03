#include "eval.h"
#include "stdio.h"
#include "malloc.h"
#include "parser.h"

const Type* generics_sub(const Type *t){
    for(generics *g = t->gen; g; g = g->next)
        if(strcmp(g->key, t->name)==0)
            return g->val;
    return t;
}

bool print_res(const Fun f, char* buff){
    const Type *valtype = generics_sub(f.type);
    if(!valtype) return false;
    if(!valtype->simple) return false;
    if(!strcmp(valtype->name, "Nat")){
        sprintf(buff, "%d", f.val->i_val);
    } else if(!strcmp(valtype->name, "Double")){
        sprintf(buff, "%f", f.val->d_val);
    } else if(!strcmp(valtype->name, "Bool")){
        if(f.val->b_val)
            sprintf(buff, "true");
        else sprintf(buff, "false");
    } else if(!strcmp(valtype->name, "Char")){
        sprintf(buff, "%c", f.val->c_val);
    } else return false;
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
    for(; t->next; t = t->next);//skip current args
    //now t has last arg
    t->next = arg;
}

eval_tree* eval_make(const Fun *f){
    eval_tree *ret = malloc(sizeof (eval_tree));
    ret->f = f;
    return ret;
}


//const Prim* eval_expr(const dict *glob, const eval_tree *input, const Prim *params);

const eval_promise* collect_args(const dict *glob, const eval_tree *tree, const eval_promise *params, unsigned int argn){
    eval_promise *args = malloc(argn * sizeof(eval_promise));
    int i = 0;
    for(const eval_tree *arg = tree->arg; arg; arg = arg->next){
        args[i++] = (eval_promise){glob, arg, params};
    }
    return args;
}


const Prim* eval_expr(const dict *glob, const eval_tree *input, const eval_promise *params){
    if(!input) return NULL;
    if(!input->f) return NULL;
    const Fun *f = input->f;

    const Prim *res = f->val;
    const eval_promise *args = collect_args(glob, input, params, input->argn);
    if(!res){
        if(*f->name == '!'){
            int argi;
            sscanf(f->name+1,"%d",&argi);
            eval_promise ep = params[argi-1];
            res = eval_expr(ep.glob, ep.input, ep.params);
        } else {
            const eval_tree *sa =  dict_get_eval(glob, f->name);
            const eval_promise *ps = collect_args(glob, input, params, sa->argn);
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

const Prim* promise_eval(eval_promise ep){
    return eval_expr(ep.glob, ep.input, ep.params);
}
