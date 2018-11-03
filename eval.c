#include "eval.h"
#include "stdio.h"
#include "malloc.h"
#include "stdio.h"
#include "parsing/expression_parser.h"
#include "processing/process.h"
#include "types.h"

/**
 * @brief Gets type that correspond to the generic type in a context
 * @param t Generic type
 * @param context Generics context
 * @return Bound type if type is generic and bound, source type otherwise
 */
const Type* generics_sub(const Type *t, generics *context){
    if(!generic(*t)) return t;
    for(generics *g = context; g; g = g->next)
        if(strcmp(g->key, t->name)==0)
            return g->val ? g->val : t;
    return t;
}
/**
 * @brief Prints an object
 * @param o Object to be printed
 */
void print_object(const object o){
    printf("%s ", o.name);
    for(unsigned int i = 0; i < o.argc; i++){
        printf("(");
        print_object(*promise_eval(o.args[i]));
        printf(")");
    }
}
/**
 * @brief Prints content of value-type function
 * @param f Function to be printed
 * @return true if function is value-type function, false otherwise
 */
bool print_res(const Fun f){
    const Type *valtype = generics_sub(f.type, f.type->gen);
    if(!valtype) return false;
    if(!valtype->simple) return false;
    print_object(*f.val);
    return true;
}
/**
 * @brief Adds argument to an evaluation tree
 * @param tree Evaluation tree
 * @param arg Argument
 */
void eval_add_arg(eval_tree *tree, eval_tree *arg){
    if(!tree){
        fprintf(stderr, "Evaluation tree not provided");
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
/**
 * @brief Makes an evaluation tree from a function
 * @param f Function
 * @return Evaluation tree
 */
eval_tree* eval_make(Fun *f){
    eval_tree *ret = calloc(1, sizeof (eval_tree));
    ret->f = f;
    return ret;
}

/**
 * @brief Creates an array of evaluation promises from arguments of a function and parameters
 * @param glob Global dictionary of names
 * @param tree Evaluation tree of a function
 * @param params Parameters
 * @param argn Number of arguments
 * @return Array of evaluation promises
 */
const eval_promise* collect_args(const dict *glob, const eval_tree *tree, const eval_promise *params, unsigned int argn){
    eval_promise *args = calloc(argn, sizeof(eval_promise));
    int i = 0;
    for(const eval_tree *arg = tree->arg; arg; arg = arg->next){
        args[i++] = (eval_promise){glob, arg, params};
    }
    return args;
}
/**
 * @brief Evaluates an expression
 * @param glob Global dictionary of names
 * @param input Evaluation tree of an expression
 * @param params Parameters
 * @return Result value
 */
object *eval_expr(const dict *glob, const eval_tree *input, const eval_promise *params){
    if(!input){
        fprintf(stderr, "Nothing to evaluate");
        return NULL;
    }
    if(!input->f){
        fprintf(stderr, "Evaluation tree is empty");
        return NULL;
    }
    const Fun *f = input->f;

    object *res = f->val;
    const eval_promise *args = collect_args(glob, input, params, input->argn);
    if(!res){
        if(f->lid){
            res = promise_eval(params[*f->lid->val]);//eval_expr(ep.glob, ep.input, ep.params);
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

/**
 * @brief Evaluates an expression from a string
 * @param glob Global dictionary of names
 * @param input Exrpression
 * @return Value-type function that contatins the result of expression evaluation and it's type
 */
Fun* eval_string(const dict *glob, const char *input){
    struct syntax_tree tl = accept_expression(&input);
    struct fun_def pr = process_app(NULL,glob, tl);
    reset_generics(pr.type);
    if(!pr.type->simple){
        fprintf(stderr, "Expression doesn't have primitive type");
        return NULL;
    }
    Fun *ret = calloc(1, sizeof (Fun));
    object *val = eval_expr(glob, pr.et->val->t, NULL);
    ret->type = pr.type;
    ret->val = val;
    return ret;
}
/**
 * @brief Evaluates an evaluation promise
 * @param ep Evaluation promise
 * @return Result value
 */
object* promise_eval(eval_promise ep){
    return eval_expr(ep.glob, ep.input, ep.params);
}
