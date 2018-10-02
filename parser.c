#include "parser.h"
#include "reader.h"
#include "eval.h"
#include <malloc.h>

parse_res parse_tan(const char *input){
    input = skip_ws(input);
    char *name = malloc(sizeof (char)*20);
    input = read_word(name, input);
    input = skip_ws(input);
    if(*input != ':'){
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
parse_res parse_left(const Fun *f, dict **local, const char *input){
    input = skip_ws(input);
    char *name = malloc(sizeof (char)*20);
    input = read_word(name, input);
    if(strcmp(name, f->name) != 0) return (parse_res){NULL,NULL, NULL};
    input = skip_ws(input);
    const Type *i = f->type;
    unsigned int lid = 1;
    while(*input != '='){
        char *id = malloc(sizeof (char)*20);
        input = read_word(id, input);
        Fun *af = malloc(sizeof (Fun));
        af->name = id;
        const Type *t = i->simple ? i : i->val.func.arg;
        af->type = t;
        af->lid = lid++;
        *local = dict_add(*local, af);
        input = skip_ws(input);
    }
    input++;
    return (parse_res){NULL,input, NULL};
}

char* rename_arg(const Fun *arg){
    char *ret = malloc(sizeof (char)*20);
    sprintf(ret,"!%d",arg->lid);
    return ret;
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

        f = apply_t(*f,*pr.type);
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
        return parse_app(local, glob, input);
    }
    char *name = malloc(sizeof (char)*20);
    input = read_word(name, input);

    const Fun *ff = dict_get(local, name);
    if(!ff)
        ff = dict_get(glob, name);

    return (parse_res){ff->type, input, eval_make(ff)};
}

parse_res parse_arg(const dict *local, const dict *glob, const char *input){
    input = skip_ws(input);
    if(*input == ')' || *input == '\n' || *input == '\0')
        return (parse_res){NULL, input, NULL};
    if(*input == '('){
        input++;
        return parse_app(local, glob, input);
    }
    Fun *ff = malloc(sizeof (Fun));
    char *name = malloc(sizeof (char)*20);
    input = read_word(name, input);
    const Fun *lt = dict_get(local, name);

    if(lt){
        ff->name = rename_arg(lt);
        ff->type = lt->type;
    } else {
        memcpy(ff, dict_get(glob, name), sizeof (Fun));
    }
    return (parse_res){ff->type, input, eval_make(ff)};
}


parse_res parse_right(const Type *f, const dict *local, const dict *glob, const char *input){
    parse_res tr = parse_app(local, glob, input);
    if(!equal_t(*last_type(f), *tr.type)) return (parse_res){NULL, NULL, NULL};
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
    dict **l = malloc(sizeof (dict*));
    parse_res b = parse_left(a.et->f,l,a.left);
    unsigned int argn = (*l) ? (**l).value->f->lid : 0;
    *l = dict_add(*l, a.et->f);
    parse_res c = parse_right(a.et->f->type, *l, glob, b.left);
    eval_tree_wrap(&c, a.et->f, argn);
    return c;
}

const dict* parse_all(const char *input){
    const dict *glob = init();
    while(*input != '\0'){
        while(*input == '\n' || *input == ')')
            input++;
        if(*input == '\0') break;
        parse_res f = parse_fun(glob, input);
        input = f.left;
        glob = dict_add_eval(glob, f.et);
    }
    return glob;
}

