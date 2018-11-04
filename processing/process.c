#include "process.h"
#include "../parsing/parser.h"
#include "../eval.h"
#include "../types.h"
#include <malloc.h>
#include "../dictionary_t.h"
#include "freemem.h"
#define PATH "/home/bgs99c/sandbox/shs/"

/**
 * @brief Finds function in dictionaries
 * @param glob Global dictionary of names
 * @param local Local dictionary of names
 * @param name Name of the function
 * @return Function on succes, NULL otherwise
 */
Fun* get_fun(const dict *glob, const arg_list *local, struct word name){
    char c = *name.begin;
    if(('0' <= c && '9' >= c) || c == '-'){
        return NULL;//parse_num(name);
    }
    Fun *ret = dict_get(glob, name);
    if(!ret){
        struct arg *loc = args_get(local, name);
        ret = loc ? loc->match : NULL;
    }

    if(!ret){
        fprintf(stderr, "Function \"%.*s\" is not found in global dictionary and arguments\n", (int)name.length, name.begin);
        return NULL;
    }
    return ret;
}
/**
 * @brief Makes pattern patch from evaluation tree
 * @param et Evaluation tree
 * @return Pattern match
 */
pattern_list *pattern_from_et(eval_tree *et){
    pattern *p = malloc(sizeof ( pattern));
    *p = (pattern){et, NULL};
    pattern_list *pl = NULL;
    list_add(pattern_list, &pl, p);
    mark_ptr(pl);
    mark_ptr(p);
    return pl;
}

struct Type *parse_t(struct syntax_tree input);
/**
 * @brief Processes struct Type annotation of a function
 * @param input Syntax tree of struct Type annotation
 * @return Function definition with right struct Type and name on success, empty struct otherwise
 */
struct fun_def process_tan(struct syntax_tree input){
    if(input.type != ANNOTATION)
        return (struct fun_def){NULL, NULL};
    struct word name = input.val;

    Fun *res = calloc(1, sizeof (Fun));
    res->name = name;
    res->type = parse_t(*input.args->val);

    eval_tree *et = eval_make(res);
    return (struct fun_def){NULL, pattern_from_et(et)};
}

struct arg *process_par(struct Type *t, struct syntax_tree input, unsigned int *lid, unsigned int depth, const dict *glob){
    struct word id = input.val;
    Fun *f = dict_get(glob, id);
    struct arg *af = calloc(1, sizeof (struct arg));
    mark_ptr(af);
    af->match = calloc(1, sizeof (Fun));
    af->match->name = id;
    af->match->type = f ? f->type : t;
    af->match->ids = lid;
    af->match->id_depth = depth;
    mark_ptr(af->match);
    if(!input.args)
        return af;
    struct Type *i = f->type;
    unsigned int liid = 0;
    for(struct tree_args *cur = input.args; cur; cur = cur->next, i = i->ret){
        unsigned int *ll = malloc(sizeof (int) * (depth + 2));
        mark_ptr(ll);
        memcpy(ll, lid, depth+1);
        ll[depth+1] = liid++;
        list_add_last(arg_list, &af->args, process_par(i->arg, *cur->val, ll, depth +1, glob));
    }
    return af;
}

/**
 * @brief process_left fills local dictionary with function parameters from input
 * @param t struct Type of the function
 * @param local dictionary of local names, 'out' parameter
 * @param input syntax tree of the function definition
 */
void process_left(struct Type *t, arg_list **local, struct syntax_tree input, const dict *glob){
    if(input.type != DEFINITION) return;
    unsigned int liid = 0;
    struct Type *at = t;
    for(struct tree_args *cur = input.args->next; cur; cur = cur->next, liid++){
        unsigned *lid = malloc(sizeof (unsigned));
        *lid = liid;
        args_add(local, process_par(at->arg, *cur->val, lid, 0, glob));
        at = at->ret;
    }
}

struct fun_def process_arg(const arg_list *local, const dict *glob, struct syntax_tree input);
struct fun_def process_f_f(const arg_list *local, const dict *glob, struct syntax_tree input);
/**
 * @brief process_app processes function application and returns function definition
 * @param local dictionary of local names
 * @param glob dictionary of global names
 * @param input syntax tree of the function definition
 * @return defined function on success, empty struct on fail
 */
struct fun_def process_app(const arg_list *local, const dict *glob, struct syntax_tree input){
    if(input.type == UNDEFINED)
        return (struct fun_def){NULL, NULL};
    struct fun_def pr = process_f_f(local, glob, input);
    struct Type *f = pr.type;
    if(!f){
        return (struct fun_def){NULL, NULL};
    }
    eval_tree* ret = pr.et->val->t;
    for(tree_args *i = input.args; i; i = i->next){
        pr = process_arg(local, glob, *i->val);
        f = apply_t(f, pr.type);
#ifdef LOGALL
        log("&&logging context\n");
        log_context(f->gen);
        log("\n");
#endif
        eval_add_arg(ret, pr.et->val->t);
    }
    return (struct fun_def){f, pattern_from_et(ret)};
}
/**
 * @brief process_f_f gets function for application
 * @param local dictionary of local names
 * @param glob dictionary of global names
 * @param input syntax tree of the function definition
 * @return function definition on success, empty struct on fail
 */
struct fun_def process_f_f(const arg_list *local, const dict *glob, struct syntax_tree input){
    if(input.type != EXPRESSION)
        return (struct fun_def){NULL,NULL};
    struct word name = input.val;

    Fun *ff = get_fun(glob, local, name);
    if(!ff)
        return (struct fun_def) {NULL, NULL};
    return (struct fun_def){ff->type, pattern_from_et(eval_make(ff))};
}
/**
 * @brief process_arg gets function argument for application
 * @param local dictionary of local names
 * @param glob dictionary of global names
 * @param input syntax tree of the function definition
 * @return arg's function definition on success, empty struct on fail
 */
struct fun_def process_arg(const arg_list *local, const dict *glob, struct syntax_tree input){
    if(input.type != EXPRESSION)
        return (struct fun_def){NULL,NULL};
    if(input.args){
        struct fun_def ret = process_app(local, glob, input);
        reset_generics(ret.type);
        return ret;
    }
    struct word name = input.val;
    Fun *ff = get_fun(glob, local, name);
    if(!ff)
        return (struct fun_def) {NULL, NULL};
    return (struct fun_def){ff->type, pattern_from_et(eval_make(ff))};
}

/**
 * @brief process_right processes right side of function's definition
 * @param f struct Type of the function
 * @param local dictionary of local names
 * @param glob dictionary of global names
 * @param input syntax tree of the expression on the right
 * @return expression's definition on success, empty struct on fail
 */
struct fun_def process_right(struct Type *f, const arg_list *local, const dict *glob, struct syntax_tree input){
    if(input.type != EXPRESSION)
        return (struct fun_def){NULL, NULL};
    struct fun_def tr = process_app(local, glob, input);
    if(!equal_t(last_type(f), generics_sub(tr.type, tr.type->gen), f->gen)){
        fprintf(stderr, "Return types do not match: \n %.*s has struct Type ",(int)f->name.length, f->name.begin);
        log_t(last_type(f));
        fprintf(stderr, "\n%.*s has struct Type ", (int)tr.et->val->t->f->name.length, tr.et->val->t->f->name.begin);
        log_t(last_type(tr.type));
        fprintf(stderr, " in a context: \n");
        log_context(tr.type->gen);
        fprintf(stderr, "\n");
        return (struct fun_def){NULL, NULL};
    }
    reset_generics(tr.type);
    return tr;
}

/**
 * @brief pattern_add adds new pattern match to function definition
 * @param pr function definition
 * @param np new pattern match
 */
void pattern_add(struct fun_def *pr, struct fun_def np){
    if(!pr->et){
        pr->et = np.et;
        return;
    }

    pattern_list *i;
    for(i = pr->et; i->next; i = i->next);

    i->next = np.et;
}

/**
 * @brief eval_tree_wrap creates evaluation tree from function
 * @param pr function definition
 * @param f function
 * @param argn number of function's arguments
 */
void eval_tree_wrap(struct fun_def *pr, Fun *f, unsigned int argn){
    eval_tree *et = eval_make(f);
    eval_add_arg(et, pr->et->val->t);
    pr->et->val->t = et;
    pr->et->val->t->argn = (int)argn;
}
/**
 * @brief pattern_add_matches generates pattern match from local dictionary
 * @param p pattern list that points to a pattern that should be generated
 * @param local local dictionary of names
 */
void pattern_add_matches(pattern_list *p, arg_list *local){
    p->val->args = local;
}
/**
 * @brief process_type generates simple struct Type from name
 * @param input syntax tree with name of the struct Type
 * @return generated struct Type
 */
struct Type* process_type(struct syntax_tree input){
    struct Type *ret = calloc(1, sizeof(struct Type));
    ret->simple = true;
    ret->name = input.val;
    return ret;
}
/**
 * @brief process_constructor adds to global dictionary a new constructor
 * @param name name of the struct Type that constructor belongs to
 * @param input syntax tree of constructor
 * @param glob global dictionary of names
 */
void process_constructor(struct Type *name, struct syntax_tree input, dict **glob){
    if(input.type != CONSTRUCTOR)
        return;
    struct word cname = input.val;
    Fun *ret = calloc(1, sizeof(Fun));
    ret->name = cname;
    object *p = malloc(sizeof (object));
    *p = (object){0, cname, NULL};
    ret->val = p;
    ret->type = name;
    struct Type *last = NULL, *first = NULL;

    for(tree_args *i = input.args; i; i = i->next){
        struct Type *arg = process_type(*i->val);
        if(!last){
            last = calloc(1, sizeof (struct Type));
            last->simple = false;
            last->arg = arg;
            first = last;
            continue;
        }
        last->ret = calloc(1, sizeof (struct Type));
        last->ret->simple = false;
        last->ret->arg = arg;
        last = last->ret;
    }
    if(first){
        ret->type = first;
        ret->type->simple = false;
        last->ret = name;
    }
    dict_add(glob, ret);
}
/**
 * @brief process_datatype adds new datatype to the global dictionary
 * @param name name of the datatype
 * @param input syntax tree of the datatype
 * @param glob global dictionary of names
 */
void process_datatype(struct Type *name, struct syntax_tree input, dict **glob){
    if(input.type != DATATYPE)
           return;
    for(tree_args *i = input.args; i; i = i->next){
        process_constructor(name, *i->val, glob);
    }
}
/**
 * @brief process_fun creates function from syntax tree
 * @param glob global dictionary of names
 * @param block syntax tree of the function
 * @return created function on success, empty struct on fail
 */
struct fun_def process_fun(const dict *glob, struct syntax_tree block){
    struct fun_def a = process_tan(*block.args->val);
#ifdef LOGALL
    log("&&Parsing function %s of struct Type ", name);
    log_t(a.et->val->t->f->type);
    log(" &&\n");
#endif
    struct fun_def c = {NULL, NULL};
    c.type = a.et->val->t->f->type;
    for(tree_args *i = block.args->next; i; i = i->next){

        arg_list **l = calloc(1, sizeof (arg_list *));
        mark_ptr(l);
        process_left(c.type, l, *i->val, glob);


        unsigned int argn = 0;

        for(arg_list *i = *l; i; i = i->next)
            argn++;

        args_add_self(l, a.et->val->t->f);

        struct fun_def t = process_right(a.et->val->t->f->type, *l, glob, *i->val->args->val);

        if(!t.type && !t.et)
            return t;

        eval_tree_wrap(&t, a.et->val->t->f, argn);
        pattern_add_matches(t.et,*l);
        pattern_add(&c, t);
    }
    return c;
}
/**
 * @brief process_text processes all text
 * @param input text to be processed
 * @param glob global dictionary of names, 'out' parameter
 */
void process_text(const char *input, dict **glob){
    struct syntax_tree tl = undefined;
    while((tl = accept_block(&input)).type != UNDEFINED){
        if(tl.type==IMPORT){
            char *path = calloc(tl.val.length + 40, sizeof (char));
            char *fn = strndup(tl.val.begin, tl.val.length);
            strcat(path, PATH);
            strcat(path, fn);
            strcat(path, ".shs");
            free(fn);
            FILE *in = fopen(path, "r");
            free(path);
            if(!in){
                printf("Cannot open file %s, halting", fn);
                syntax_tree_free(tl);
                return;
            }
            fseek(in, 0, SEEK_END);
            unsigned long len = (unsigned long)ftell(in);
            char *all = calloc(len+1, sizeof (char));
            mark_ptr(all);
            rewind(in);
            fread(all, 1, len, in);
            fclose(in);

            process_text(all, glob);
            syntax_tree_free(tl);
            continue;
        }
        if(tl.type == DATATYPE){
            struct Type *t = type_make(tl.val);
            process_datatype(t, tl, glob);
            syntax_tree_free(tl);
            continue;
        }
        struct fun_def f = process_fun(*glob, tl);
        list_add(dict, glob, f.et);
        syntax_tree_free(tl);
    }
}
/**
 * @brief process_all generates global dictionary by processing all text
 * @param input text to be processed
 * @return global dictionary of names
 */
dict* process_all(const char *input){
    dict **glob = calloc(1, sizeof (dict *));
    process_text(input, glob);
    dict *ret = *glob;
    free(glob);
    glob = NULL;
    return ret;
}
