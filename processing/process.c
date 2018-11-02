#include "process.h"
#include "../parsing/parser.h"
#include "../eval.h"
#include <malloc.h>

const Fun* get_fun(const dict *glob, const dict *local, const char *name){
    char c = *name;
    if(('0' <= c && '9' >= c) || c == '-'){
        return NULL;//parse_num(name);
    }

    const Fun *ret = dict_get(local, name);
    if(!ret)
        ret = dict_get(glob, name);
    if(!ret){
        fprintf(stderr, "Function \"%s\" is not found in global dictionary and arguments", name);
        return NULL;
    }
    return ret;
}

pattern_list* pattern_from_et(eval_tree *et){
    pattern *p = calloc(1, sizeof (pattern));
        *p = (pattern){et, NULL};
        pattern_list *ret = malloc(sizeof (pattern_list));
        *ret = list_create(pattern_list, p);
    return ret;
}

Parsed parse_t(struct syntax_tree input);

struct fun_def parse_tan(struct syntax_tree input){
    if(input.type != ANNOTATION) return (struct fun_def){NULL,NULL};
    const char *name = get_name(input);
    Fun *res = calloc(1, sizeof (Fun));
    res->name = name;
    Parsed p = parse_t(*input.args->val);
    res->type = p.ret;
    eval_tree *et = eval_make(res);
    return (struct fun_def){NULL, pattern_from_et(et)};
}
void parse_left(const Fun *f, dict **local, struct syntax_tree input){
    if(input.type == DEFINITION) return;
    const char *name = get_name(input);
    if(strcmp(name, f->name) != 0){
        fprintf(stderr, "Annotation's name \"%s\" and definition name \"%s\" do not match", name, f->name);
    }
    Type *i = f->type;
    unsigned int lid = 0;
    struct tree_args *cur;
    for(cur = input.args; cur->val->type != EXPRESSION; cur = cur->next){
        const char *id = get_name(*cur->val);
        Fun *af = calloc(1, sizeof (Fun));
        af->name = id;
        //const Type *t = i->simple ? i : i->arg;
        af->type = i->arg;
        i = i->ret;
        af->lid = calloc(1, sizeof (id_list));
        af->lid->val = malloc(sizeof (unsigned int));
        *af->lid->val = lid++;
        dict_add(local, af);
    }
}

struct fun_def process_arg(const dict *local, const dict *glob, struct syntax_tree input);
struct fun_def process_f_f(const dict *local, const dict *glob, struct syntax_tree input);

struct fun_def process_app(const dict *local, const dict *glob, struct syntax_tree input){
    if(input.type == UNDEFINED) return (struct fun_def){NULL,NULL};
    struct fun_def pr = process_f_f(local, glob, input);
    const Type *f = pr.type;
    eval_tree *ret = pr.et->val->t;
    while((pr = process_arg(local, glob, input)).type){

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

struct fun_def process_f_f(const dict *local, const dict *glob, struct syntax_tree input){
    if(input.type != EXPRESSION)
        return (struct fun_def){NULL,NULL};
    const char *name = get_name(input);

    const Fun *ff = get_fun(glob, local, name);
    return (struct fun_def){ff->type, pattern_from_et(eval_make(ff))};
}

struct fun_def process_arg(const dict *local, const dict *glob, struct syntax_tree input){
    if(input.type != EXPRESSION)
        return (struct fun_def){NULL,NULL};
    if(input.args){
        struct fun_def ret = process_app(local, glob, input);
        return ret;
    }
    const char *name = get_name(input);
    const Fun *ff = get_fun(glob, local, name);
    return (struct fun_def){ff->type, pattern_from_et(eval_make(ff))};
}


struct fun_def parse_right(const Type *f, const dict *local, const dict *glob, struct syntax_tree input){
    if(input.type != EXPRESSION)
        return (struct fun_def){NULL,NULL};
    struct fun_def tr = process_app(local, glob, input);
    if(!equal_t(last_type(f), generics_sub(tr.type, tr.type->gen), tr.type->gen)){
        fprintf(stderr, "Return types do not match: \n %s has type ", f->name);
        log_t(last_type(f));
        fprintf(stderr, "\n%s has type ", tr.et->val->t->f->name);
        log_t(last_type(tr.type));
        fprintf(stderr, " in a context: \n");
        log_context(tr.type->gen);
        fprintf(stderr, "\n");
        return (struct fun_def){NULL, NULL};
    }
    return tr;
}
