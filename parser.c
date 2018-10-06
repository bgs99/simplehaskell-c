#include "parser.h"
#include "reader.h"
#include "eval.h"
#include <malloc.h>

const Fun* get_fun(const dict *glob, const dict *local, char *name){
    const Fun *ret = dict_get(local, name);
    if(!ret)
        ret = dict_get(glob, name);
    if(!ret){
        log("Function \"%s\" is not found in global dictionary and arguments", name);
        return NULL;
    }
    return ret;
}


parse_res parse_tan(const char *input){
    input = skip_ws(input);
    char *name = calloc(20, sizeof (char));
    input = read_word(name, input);
    input = skip_ws(input);
    if(*input != ':'){
        log("Expected \':\' in type annotation of %s", name);
        return (parse_res){NULL,NULL, NULL};
    }
    input++;
    Fun *res = malloc(sizeof (Fun));
    res->name = name;
    Parsed p = parse_t(input);
    res->type = p.ret;
    eval_tree *et = eval_make(res);
    return (parse_res){NULL, p.left+1, et};
}
parse_res parse_left(const Fun *f,const dict **local, const char *input){
    input = skip_ws(input);
    char *name = calloc(20, sizeof (char));
    input = read_word(name, input);
    if(strcmp(name, f->name) != 0){
        log("Annotation's name \"%s\" and definition name \"%s\" do not match", name, f->name);
        return (parse_res){NULL,NULL, NULL};
    }
    input = skip_ws(input);
    const Type *i = f->type;//--
    unsigned int lid = 1;
    while(*input != '='){
        char *id = calloc(20, sizeof (char));
        input = read_word(id, input);
        Fun *af = malloc(sizeof (Fun));
        af->name = id;
        //const Type *t = i->simple ? i : i->arg;
        af->type = i->arg;

        i = i->ret;

        af->lid = lid++;
        dict_add(local, af);
        input = skip_ws(input);

    }
    input++;
    return (parse_res){NULL,input, NULL};
}

parse_res parse_arg(const dict *local, const dict *glob, const char *input);
parse_res parse_f_f(const dict *local, const dict *glob, const char *input);

parse_res parse_app(const dict *local, const dict *glob, const char *input){
    parse_res pr = parse_f_f(local, glob, input);
    input = pr.left;
    const Type *f = pr.type;
    eval_tree *ret = pr.et;
    while((pr = parse_arg(local, glob, input)).type){
        input = pr.left;

        f = apply_t(f, pr.type);
#ifdef LOGALL
        log("&&logging context\n");
        log_context(f->gen);
        log("\n");
#endif
        eval_add_arg(ret, pr.et);
    }
    input = pr.left;
    return (parse_res){f, input, ret};
}
parse_res parse_f_f(const dict *local, const dict *glob, const char *input){
    input = skip_ws(input);
    if(*input == ')' || *input == '\n' || *input == '\0')
        return (parse_res){NULL, input, NULL};
    if(*input == '('){
        input++;
        parse_res ret = parse_app(local, glob, input);
        if(*ret.left == ')') ret.left++;
        return ret;
    }
    char *name = calloc(20, sizeof (char));
    input = read_word(name, input);

    const Fun *ff = get_fun(glob, local, name);
    return (parse_res){ff->type, input, eval_make(ff)};
}

parse_res parse_arg(const dict *local, const dict *glob, const char *input){
    input = skip_ws(input);
    if(*input == ')' || *input == '\n' || *input == '\0')
        return (parse_res){NULL, input, NULL};
    if(*input == '('){
        input++;
        parse_res ret = parse_app(local, glob, input);
        if(*ret.left == ')') ret.left++;
        return ret;
    }
    char *name = calloc(20, sizeof (char));
    input = read_word(name, input);
    const Fun *ff = get_fun(glob, local, name);
    return (parse_res){ff->type, input, eval_make(ff)};
}


parse_res parse_right(const Type *f, const dict *local, const dict *glob, const char *input){
    parse_res tr = parse_app(local, glob, input);
    if(!equal_t(last_type(f), generics_sub(tr.type, tr.type->gen), tr.type->gen)){
        log("Return types do not match: \n %s has type ", f->name);
        log_t(last_type(f));
        log("\n%s has type ", tr.et->f->name);
        log_t(last_type(tr.type));
        log(" in a context: \n");
        log_context(tr.type->gen);
        log("\n");
        return (parse_res){NULL, NULL, NULL};
    }
    return tr;
}

void eval_tree_wrap(parse_res *pr, const Fun *f, unsigned int argn){
    eval_tree *et = eval_make(f);
    eval_add_arg(et, pr->et);
    pr->et = et;
    pr->et->argn = argn;
}

parse_res parse_fun(const dict *glob, const char *input){
    parse_res a = parse_tan(input);
    const dict **l = malloc(sizeof (dict*));
#ifdef LOGALL
    log("&&Parsing function %s of type ",a.et->f->name);
    log_t(a.et->f->type);
    log(" &&\n");
#endif
    parse_res b = parse_left(a.et->f,l,a.left);
    unsigned int argn = (*l) ? (**l).value->f->lid : 0;
    dict_add(l, a.et->f);
    parse_res c = parse_right(a.et->f->type, *l, glob, b.left);
    eval_tree_wrap(&c, a.et->f, argn);
    return c;
}

const dict* parse_all(const char *input){
    const dict *glob = init();
    while(input && *input != '\0'){

        while(*input == '\n' || *input == ')' || *input == ' ' || *input == '\t')
            input++;
        if(*input == '\0') break;
        parse_res f = parse_fun(glob, input);
        input = f.left;
        dict_add_eval(&glob, f.et);
    }
    return glob;
}

