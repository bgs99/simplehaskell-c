#include "eval.h"
#include "stdio.h"
#include "malloc.h"
#include "parser.h"

const Type* generics_sub(const Type *t, generics *context){
    if(!generic(*t)) return t;
    for(generics *g = context; g; g = g->next)
        if(strcmp(g->key, t->name)==0)
            return g->val ? g->val : t;
    return t;
}

bool print_res(const Fun f){
    const Type *valtype = generics_sub(f.type, f.type->gen);
    if(!valtype) return false;
    if(!valtype->simple) return false;
    if(!strcmp(valtype->name, "Int")){
        printf("%d", f.val->i_val);
    } else if(!strcmp(valtype->name, "Double")){
        printf("%f", f.val->d_val);
    } else if(!strcmp(valtype->name, "Bool")){
        if(f.val->b_val)
            printf("true");
        else printf("false");
    } else if(!strcmp(valtype->name, "Char")){
        printf("%c", f.val->c_val);
    } else return false;
    return true;
}

void eval_add_arg(eval_tree *tree, eval_tree *arg){
    if(!tree){
        log("Evaluation tree not provided");
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
    eval_promise *args = calloc(argn, sizeof(eval_promise));
    int i = 0;
    for(const eval_tree *arg = tree->arg; arg; arg = arg->next){
        args[i++] = (eval_promise){glob, arg, params};
    }
    return args;
}


const Prim* eval_expr(const dict *glob, const eval_tree *input, const eval_promise *params){
    if(!input){
        log("Nothing to evaluate");
        return NULL;
    }
    if(!input->f){
        log("Evaluation tree is empty");
        return NULL;
    }
    const Fun *f = input->f;

    const Prim *res = f->val;
    const eval_promise *args = collect_args(glob, input, params, input->argn);
    if(!res){
        if(f->lid > 0){
            res = promise_eval(params[f->lid-1]);//eval_expr(ep.glob, ep.input, ep.params);
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
    const token_list *tl = tokenize(&input);
    parse_res pr = parse_app(NULL,glob, &tl);
    if(!pr.type->simple){
        log("Expression doesn't have primitive type");
        return NULL;
    }
    Fun *ret = malloc(sizeof (Fun));
    const Prim *val = eval_expr(glob, pr.et->t, NULL);
    ret->type = pr.type;
    ret->val = val;
    return ret;
}

const Prim* promise_eval(eval_promise ep){
    return eval_expr(ep.glob, ep.input, ep.params);
}
