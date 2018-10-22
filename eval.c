#include "eval.h"
#include "stdio.h"
#include "malloc.h"
#include "parser.h"
#include "stdio.h"

const Type* generics_sub(const Type *t, generics *context){
    if(!generic(*t)) return t;
    for(generics *g = context; g; g = g->next)
        if(strcmp(g->key, t->name)==0)
            return g->val ? g->val : t;
    return t;
}

void print_object(const object o){
    printf("%s ", o.name);
    for(unsigned int i = 0; i < o.argc; i++){
        printf("(");
        print_object(*promise_eval(o.args[i]));
        printf(")");
    }
}

bool print_res(const Fun f){
    const Type *valtype = generics_sub(f.type, f.type->gen);
    if(!valtype) return false;
    if(!valtype->simple) return false;
    print_object(*f.val);
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
    eval_tree *ret = calloc(1, sizeof (eval_tree));
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

object *eval_expr(const dict *glob, const eval_tree *input, const eval_promise *params){
    if(!input){
        log("Nothing to evaluate");
        return NULL;
    }
    if(!input->f){
        log("Evaluation tree is empty");
        return NULL;
    }
    const Fun *f = input->f;

    object *res = f->val;
    const eval_promise *args = collect_args(glob, input, params, input->argn);
    if(!res){
        if(f->lid){
            res = promise_eval(params[*f->lid->val-1]);//eval_expr(ep.glob, ep.input, ep.params);
        } else {
            const eval_tree *sa =  dict_get_eval(glob, f->name, args);
            const eval_promise *ps = collect_args(glob, input, params, sa->argn);
            return eval_expr(glob, sa->arg, ps);
        }
    }
    if(!input->arg){
        return res;
    }
    res->args = args;
    res->argc = input->argn;
    return res;
}

Fun* eval_string(const dict *glob, const char *input){
    token_list *tl = tokenize(&input);
    parse_res pr = parse_app(NULL,glob, &tl);
    if(!pr.type->simple){
        log("Expression doesn't have primitive type");
        return NULL;
    }
    Fun *ret = calloc(1, sizeof (Fun));
    object *val = eval_expr(glob, pr.et->val->t, NULL);
    ret->type = pr.type;
    ret->val = val;
    return ret;
}

object* promise_eval(eval_promise ep){
    return eval_expr(ep.glob, ep.input, ep.params);
}
