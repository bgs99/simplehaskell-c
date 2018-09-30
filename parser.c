#include "parser.h"
#include "reader.h"
#include <malloc.h>

parse_res parse_tan(const char *input){
    input = skip_ws(input);
    char *name = malloc(sizeof (char)*20);
    input = read_word(name, input);
    input = skip_ws(input);
    if(*input != ':'){
        return (parse_res){NULL,NULL};
    }
    input++;
    Fun *res = malloc(sizeof (Fun));
    res->name = name;
    Parsed p = parse_t(input);
    res->type = p.ret;
    return (parse_res){res, p.left+1};
}
parse_res parse_left(Fun *f, dict **local, const char *input){
    input = skip_ws(input);
    char *name = malloc(sizeof (char)*20);
    input = read_word(name, input);
    if(strcmp(name, f->name)) return (parse_res){NULL,NULL};
    input = skip_ws(input);
    const Type *i = f->type;
    while(*input != '='){
        char *id = malloc(sizeof (char)*20);
        input = read_word(id, input);
        Fun *af = malloc(sizeof (Fun));
        af->name = id;
        const Type *t = i->simple ? i : i->val.func.arg;
        af->type = t;
        *local = dict_add(*local, af);
        input = skip_ws(input);
    }
    input++;
    return (parse_res){f,input};
}


parse_res parse_arg(const dict *local, const char *input);
parse_res parse_f_f(const dict *local, const char *input);

parse_res parse_app(const dict *local, const char *input){
    parse_res pr = parse_f_f(local, input);
    input = pr.left;
    Fun *f = pr.val;
    while((pr = parse_arg(local, input)).val){
        input = pr.left;
        f->type = apply_t(*f->type,*pr.val->type);
    }
    input = pr.left;
    return (parse_res){f, input};
}
parse_res parse_f_f(const dict *local, const char *input){
    input = skip_ws(input);
    if(*input == ')' || *input == '\n' || *input == '\0')
        return (parse_res){NULL, input};
    if(*input == '('){
        input++;
        return parse_app(local, input);
    }
    Fun *ff = malloc(sizeof (Fun));
    char *name = malloc(sizeof (char)*20);
    input = read_word(name, input);
    ff->name = name;
    ff->type = dict_get(local, name)->type;
    return (parse_res){ff, input};
}

parse_res parse_arg(const dict *local, const char *input){
    input = skip_ws(input);
    if(*input == ')' || *input == '\n' || *input == '\0')
        return (parse_res){NULL, input};
    if(*input == '('){
        input++;
        return parse_app(local, input);
    }
    Fun *ff = malloc(sizeof (Fun));
    char *name = malloc(sizeof (char)*20);
    input = read_word(name, input);
    ff->name = name;
    ff->type = dict_get(local, name)->type;
    return (parse_res){ff, input};
}


parse_res parse_right(Fun *f, const dict *local, const char *input){
    parse_res tr = parse_app(local, input);
    if(!equal_t(*last_type(f->type), *tr.val->type)) return (parse_res){NULL, NULL};
    return (parse_res){f, tr.left};
}

parse_res parse_fun(const char *input){
    parse_res a = parse_tan(input);
    dict *l[1];
    parse_res b = parse_left(a.val,l,a.left);
    *l = dict_add(*l, b.val);
    parse_res c = parse_right(b.val, *l,b.left);
    char * def = malloc(sizeof (char)*256);
    strncpy(def,a.left,(size_t)(c.left-a.left));
    c.val->def = def;
    return c;
}


