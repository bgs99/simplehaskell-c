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
parse_res parse_right(Fun *f, const dict *local, char * input);

parse_res parse_fun(const char *tan, const char *def);
