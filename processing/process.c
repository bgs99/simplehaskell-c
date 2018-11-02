#include "process.h"
#include "../parsing/parser.h"
#include "../eval.h"
#include "../types.h"
#include <malloc.h>
#define PATH "/home/bgs99c/sandbox/shs/"

bool is_const(const char *input){
    char c = *input;
    return (c >= '0' && c <='9') || (c >= 'A' && c <= 'Z');
}

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

struct fun_def process_tan(struct syntax_tree input){
    if(input.type != ANNOTATION) return (struct fun_def){NULL,NULL};
    const char *name = get_name(input);
    Fun *res = calloc(1, sizeof (Fun));
    res->name = name;
    Parsed p = parse_t(*input.args->val);
    res->type = p.ret;
    eval_tree *et = eval_make(res);
    return (struct fun_def){NULL, pattern_from_et(et)};
}
void process_left(const Fun *f, dict **local, struct syntax_tree input){
    if(input.type != DEFINITION) return;
    Type *i = f->type;
    unsigned int lid = 0;
    struct tree_args *cur;
    for(cur = input.args->next; cur; cur = cur->next){
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
    Type *f = pr.type;
    eval_tree *ret = pr.et->val->t;
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
        reset_generics(ret.type);
        return ret;
    }
    const char *name = get_name(input);
    const Fun *ff = get_fun(glob, local, name);
    return (struct fun_def){ff->type, pattern_from_et(eval_make(ff))};
}


struct fun_def process_right(const Type *f, const dict *local, const dict *glob, struct syntax_tree input){
    if(input.type != EXPRESSION)
        return (struct fun_def){NULL,NULL};
    struct fun_def tr = process_app(local, glob, input);
    if(!equal_t(last_type(f), generics_sub(tr.type, tr.type->gen), f->gen)){
        fprintf(stderr, "Return types do not match: \n %s has type ", f->name);
        log_t(last_type(f));
        fprintf(stderr, "\n%s has type ", tr.et->val->t->f->name);
        log_t(last_type(tr.type));
        fprintf(stderr, " in a context: \n");
        log_context(tr.type->gen);
        fprintf(stderr, "\n");
        return (struct fun_def){NULL, NULL};
    }
    reset_generics(tr.type);
    return tr;
}


void pattern_add(struct fun_def *pr, struct fun_def np){
    if(!pr->et){
        pr->et = np.et;
        return;
    }

    pattern_list *i;
    for(i = pr->et; i->next; i = i->next);

    i->next = np.et;
}

void eval_tree_wrap(struct fun_def *pr, const Fun *f, unsigned int argn){
    eval_tree *et = eval_make(f);
    eval_add_arg(et, pr->et->val->t);
    pr->et->val->t = et;
    pr->et->val->t->argn = argn;
}

void pattern_add_matches(pattern_list *p, const dict *local){
    p->val->match = calloc(p->val->t->argn, sizeof (Fun));
    unsigned int j = p->val->t->argn-1;
    for(const dict *i = local->next; i; i = i->next, j--){
        if(i->val->val->t->f->name){
            if(is_const(i->val->val->t->f->name))
                p->val->match[j] = i->val->val->t->f;
        }
    }
}
Type* process_type(struct syntax_tree input){
    Type *ret = calloc(1, sizeof(Type));
    ret->simple = true;
    ret->name = get_name(input);
    return ret;
}

void process_constructor(Type *name, struct syntax_tree input, dict **glob){
    if(input.type != CONSTRUCTOR)
        return;
    const char *cname = get_name(input);
    Fun *ret = calloc(1, sizeof(Fun));
    ret->name = cname;
    object *p = malloc(sizeof (object));
    *p = (object){0, cname, NULL};
    ret->val = p;
    ret->type = name;
    Type *last = NULL, *first = NULL;

    for(tree_args *i = input.args; i; i = i->next){
        Type *arg = process_type(*i->val);
        if(!last){
            last = calloc(1, sizeof (Type));
            last->simple = false;
            last->arg = arg;
            first = last;
            continue;
        }
        last->ret = calloc(1, sizeof (Type));
        last->ret->simple = false;
        last->ret->arg = arg;
    }
    if(first){
        ret->type = first;
        ret->type->simple = false;
        last->ret = name;
    }
    dict_add(glob, ret);
}

void process_datatype(Type *name, struct syntax_tree input, dict **glob){
    if(input.type != DATATYPE)
           return;
    for(tree_args *i = input.args; i; i = i->next){
        process_constructor(name, *i->val, glob);
    }
}
struct fun_def process_fun(const dict *glob, struct syntax_tree block){
    struct fun_def a = process_tan(*block.args->val);
#ifdef LOGALL
    log("&&Parsing function %s of type ", name);
    log_t(a.et->val->t->f->type);
    log(" &&\n");
#endif
    struct fun_def c = {NULL, NULL};
    c.type = a.et->val->t->f->type;
    for(tree_args *i = block.args->next; i; i = i->next){

        dict **l = calloc(1, sizeof (dict *));

        process_left(a.et->val->t->f, l, *i->val);
        unsigned int argn = (*l) ? *(**l).val->val->t->f->lid->val + 1 : 0;
        dict_add(l, a.et->val->t->f);

        struct fun_def t = process_right(a.et->val->t->f->type, *l, glob, *i->val->args->val);

        eval_tree_wrap(&t, a.et->val->t->f, argn);
        pattern_add_matches(t.et,*l);
        pattern_add(&c, t);
    }
    return c;
}

void process_text(const char *input, dict **glob){
    struct syntax_tree tl = undefined;
    while((tl = accept_block(&input)).type != UNDEFINED){
        if(tl.type==IMPORT){
            char *path = calloc(tl.val.length + 26, sizeof (char));
            const char *fn = get_name(tl);
            strcat(path, PATH);
            strcat(path, fn);
            strcat(path, ".shs");
            FILE *in = fopen(path, "r");
            if(!in){
                printf("Cannot open file %s, halting", fn);
                return;
            }
            fseek(in, 0, SEEK_END);
            unsigned long len = (unsigned long)ftell(in);
            char *all = calloc(len, sizeof (char));
            rewind(in);
            fread(all, 1, len, in);
            fclose(in);

            process_text(all, glob);
            continue;
        }
        if(tl.type == DATATYPE){
            const char *name = get_name(tl);
            Type *t = type_make(name);
            process_datatype(t, tl, glob);
            continue;
        }
        struct fun_def f = process_fun(*glob, tl);
        list_add(dict, glob, f.et);
    }
}

dict* process_all(const char *input){
    dict **glob = calloc(1, sizeof (dict *));
    process_text(input, glob);
    return *glob;
}
