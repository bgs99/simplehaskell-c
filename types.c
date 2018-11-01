#include "types.h"
#include <malloc.h>
#include "parsing/parser.h"
#include "dictionary_t.h"
#include "eval.h"

const char* alloc_name(const char* name){
    unsigned int len = 0;
    for(const char *i = name; *i !='\0';i++)
        len++;
    char *ret = calloc(len+1, sizeof (char));
    strcpy(ret, name);
    return ret;
}

bool equal_t(const Type *a, const Type *b, generics *context){
    if(a->simple && generic(*a)){
        if(b->simple && generic(*b) && strcmp(a->name, b->name) == 0)
            return true;
        return generics_bind(context, a->name, b);
    }
    if(a->simple != b->simple) return false;//if a is function and b is val
    if(a->simple)
        return !strcmp(a->name, b->name);//if both val compare names

    if(!equal_t(a->arg, b->arg, context)) return false;//if args are different
    return equal_t(a->ret, b->ret, context);//comparing return types
}

Type* apply_t(const Type *a, const Type *b){
    if(a->simple){
        fprintf(stderr, "Trying to apply function of type ");
        log_t(b);
        fprintf(stderr, "to a primitive value of type ");
        log_t(a);
        return NULL;
    }
    if(!equal_t(a->arg, b, a->gen)){
        fprintf(stderr, "Function of type ");
        log_t(b);
        fprintf(stderr, " cannot be assigned to a function of type ");
        log_t(a);
        fprintf(stderr, " in a context: ");
        log_context(a->gen);
        return NULL;
    }
    a->ret->gen = a->gen;
    return a->ret;
}


Parsed _parse_arg(struct syntax_tree **input){
    Parsed res;
    /*if((*input)->val->type == UNDEFINED){
        return parse_t(input);
    }
    res.ret = calloc(1, sizeof (Type));
    res.ret->simple = true;
    const char *name = get_name(input);
    res.ret->name = name;
    if(generic(*res.ret))
        generics_add(res.ret, name);*/
    return res;
}
Parsed _parse_ret(struct syntax_tree **input){
    if(!*input)
        return (Parsed){NULL, NULL};
    /*if((*input)->val->type == UNDEFINED){
        *input = (*input)->next;
        return parse_t(input);
    }
    if((*input)->val->type == UNDEFINED)
        *input = (*input)->next;*/
    return (Parsed){NULL, NULL};
}

Parsed parse_t(struct syntax_tree **input){
    Parsed res;
    res.ret = calloc(1, sizeof (Type));
    Parsed arg, ret;
    /*switch((*input)->val->type){
        case UNDEFINED:
            *input = (*input)->next;
            arg = parse_t(input);
        break;
        default:
            arg = _parse_arg(input);
    }
    res.ret->arg = arg.ret;
    res.ret->simple = false;
    generics_merge(res.ret, arg.ret);
    ret = _parse_ret(input);
    if(!ret.ret){
        res.ret = arg.ret;
    } else {
        generics_merge(res.ret, ret.ret);
        res.ret->ret = ret.ret;
    }*/
    return res;
}

void fprint_t(const Type *t, FILE *dest){
    if(!t){
        fprintf(dest, "No type provided");
        return;
    }
    if(t->simple){
        fprintf(dest, "%s", t->name);
        return;
    }
    if(t->arg->simple){
        fprint_t(t->arg, dest);
    } else {
        fprintf(dest, "(");
        fprint_t(t->arg, dest);
        fprintf(dest, ")");
    }
    if(!t->ret) return;
    fprintf(dest, " - ");
    fprint_t(t->ret, dest);
}


const Type* last_type(const Type *t){
    if(t->simple || !t->ret)
        return t;
    return last_type(t->ret);
}


bool generic(Type t){
    if(!t.simple) return false;
    char c = *t.name;
    return (c >= 'a') && (c <= 'z');
}

void fprint_context(generics *g, FILE *f){
    if(!g) {
        fprintf(f, "No type vars");
        return;
    }
    for(generics *i = g; i; i = i->next){
        fprintf(f, "\t%s = ",g->key);
        fprint_t(g->val, f);
        fprintf(f, "\n");
    }
}


Type* type_make(const char *name){
    const char *id = alloc_name(name);
    Type *ret = calloc(1, sizeof (Type));
    ret->simple = true;
    ret->name = id;
    if(generic(*ret))
        generics_add(ret, id);
    return ret;
}


Type* type_add(Type *fun, Type *arg){
    generics_merge(fun, arg);
    if(fun->simple){
        Type *r = calloc(1, sizeof (r));
        r->arg = fun;
        r->ret = arg;
        r->simple = false;
        return r;
    }
    Type *i;
    for(i = fun; !i->ret->simple; i = i->ret);
    Type *r = calloc(1, sizeof (r));
    r->arg = i->ret;
    r->simple = false;
    r->ret = arg;
    i->ret = r;
    return fun;
}

bool object_equal(object a, object b){
    if(a.argc != b.argc)
        return false;
    if(strcmp(a.name, b.name) != 0)
        return false;
    for(unsigned i = 0; i < a.argc; i++){
        object x = *promise_eval(a.args[i]);
        object y = *promise_eval(b.args[i]);
        if(!object_equal(x, y))
            return false;
    }
    return true;
}
