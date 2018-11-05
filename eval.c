#include "eval.h"
#include "stdio.h"
#include "malloc.h"
#include "stdio.h"
#include "parsing/expression_parser.h"
#include "processing/process.h"
#include "types.h"
#include "freemem.h"
#include "pretty.h"

/**
 * @brief Gets struct Type that correspond to the generic struct Type in a context
 * @param t Generic struct Type
 * @param context Generics context
 * @return Bound struct Type if struct Type is generic and bound, source struct Type otherwise
 */
struct Type* generics_sub(struct Type *t, generics *context){
    if(!generic(*t)) return t;
    for(generics *g = context; g; g = g->next)
        if(name_equal(g->key, t->name))
            return g->val ? g->val : t;
    return t;
}
/**
 * @brief Prints an object
 * @param o Object to be printed
 */
void print_object(const object o){
    if(name_is(o.name, "S") || name_is(o.name, "Z")){
        printf("%d", get_nat(o));
        return;
    }

    if(name_is(o.name, "Cons") || name_is(o.name, "Nil")){
        print_list(o);
        return;
    }

    if(o.argc)
        printf("(");
    printf("%.*s", (int)o.name.length, o.name.begin);
    for(int i = 0; i < o.argc; i++){
        printf(" ");
        eval_promise *ep = o.args + i;
        object *x = promise_eval(ep);
        if(!x){
            printf("\n\n");
            return;
        }
        print_object(*x);
        fflush(stdout);
    }

    if(o.argc)
        printf(")");
}
/**
 * @brief Prints content of value-struct Type function
 * @param f Function to be printed
 * @return true if function is value-struct Type function, false otherwise
 */
bool print_res(const Fun *f){
    if(!f)
        return false;
    const struct Type *valtype = generics_sub(f->type, f->type->gen);
    if(!valtype) return false;
    if(!valtype->simple) return false;
    print_object(*f->val);
    return true;
}
/**
 * @brief Adds argument to an evaluation tree
 * @param tree Evaluation tree
 * @param arg Argument
 */
void eval_add_arg(eval_tree *tree, eval_tree *arg){
    if(!tree){
        fprintf(stderr, "Evaluation tree not provided\n");
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
eval_tree *eval_make(Fun *f){
    eval_tree *ret = malloc(sizeof (eval_tree));
    *ret = (eval_tree){f, 0, NULL, NULL};
    mark_ptr(ret);
    return ret;
}

/**
 * @brief Creates an array of evaluation promises from arguments of a function and parameters
 * @param glob Global dictionary of names
 * @param tree Evaluation tree of a function
 * @param params Parameters
 * @param argn Number of arguments
 * @return Evaluation promises of arguments
 */
eval_promise* extract_var(unsigned depth, const unsigned *lid, eval_promise *param){
    if(!param){
        fprintf(stderr, "\nExpected parameter for extraction\n");
        return NULL;
    }
    if(depth == 0)
        return param;
    if(!param->val)
        promise_eval(param);
    return extract_var(depth-1, lid+1, param->val->args + (*lid));
}
eval_promise* collect_args(dict *glob, const eval_tree *tree, eval_promise *params, int argn, unsigned parn){
    if(argn == 0)
        return NULL;
    eval_promise *args = calloc((unsigned)argn, sizeof(eval_promise));
    mark_ptr(args);
    int i = 0;
    for(eval_tree *arg = tree->arg; arg; arg = arg->next, i++){
        args[i] = (eval_promise){glob, arg, params, parn, NULL};
    }
    return args;
}


/**
 * @brief Evaluates an expression
 * @param glob Global dictionary of names
 * @param input Evaluation tree of an expression
 * @param params Passed values
 * @return Result value
 */
object *eval_expr(dict *glob, const eval_tree *input, eval_promise *params, unsigned parn){
    if(!input){
        fprintf(stderr, "Nothing to evaluate\n");
        return NULL;
    }
    if(!input->f){
        fprintf(stderr, "Evaluation tree is empty\n");
        return NULL;
    }
    const Fun *f = input->f;

    eval_promise *args = collect_args(glob, input, params, input->argn, parn);
    if(!args && input->argn)
        return NULL;
    if(f->ids){//if it is variable
        eval_promise *ep = extract_var(f->id_depth, f->ids+1, params + (*f->ids));//get it from args
        if(!ep)
            return NULL;
        return promise_eval(ep);//and return it's value
    }
    object *res = f->val;
    if(!res){//if function is not constant
        const eval_tree *sa =  dict_get_eval(glob, f->name, args);//find how to calculate it
        if(!sa){
            fprintf(stderr, "Pattern matching for function %.*s failed\n", (int)f->name.length, f->name.begin);
            return NULL;
        }
        return eval_expr(glob, sa->arg, args, (unsigned)input->argn);//and perform the calculation
    }
    if(!input->arg){
        return res;
    }
    struct word name = res->name;
    res = malloc(sizeof (object));
    mark_ptr(res);
    res->name = name;
    res->args = args;
    res->argc = (int)input->argn;
    return res;
}

/**
 * @brief Evaluates an expression from a string
 * @param glob Global dictionary of names
 * @param input Exrpression
 * @return Value-struct Type function that contatins the result of expression evaluation and it's struct Type
 */
Fun* eval_string(dict *glob, const char *input){
    struct syntax_tree tl = accept_expression(&input);
    struct fun_def pr = process_app(NULL,glob, tl);
    if(!pr.type){
        fprintf(stderr, "Cannot evaluate\n");
        return NULL;
    }
    reset_generics(pr.type);
    if(!pr.type->simple){
        fprintf(stderr, "Expression doesn't have primitive type\n");
        return NULL;
    }
    Fun *ret = calloc(1, sizeof (Fun));
    object *val = eval_expr(glob, pr.et->val->t, NULL, 0);
    if(!val)
        return NULL;
    ret->type = pr.type;
    ret->val = val;
    return ret;
}

/**
 * @brief Evaluates an evaluation promise
 * @param ep Evaluation promise
 * @return Result value
 */
object* promise_eval(eval_promise *ep){
    if(ep->val)
        return ep->val;
    object *ret = eval_expr(ep->glob, ep->input, ep->params, ep->paramc);
    if(!ret){
        fprintf(stderr, "Evaluation of function %.*s has failed", (int)ep->input->f->name.length, ep->input->f->name.begin);
        return NULL;
    }
    ep->val = ret;
    return ret;
}
