#include "types.h"
#include <malloc.h>
#include "reader.h"
#include "dictionary_t.h"

const char* alloc_name(const char* name, unsigned int len){
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

const Type* apply_t(const Type *a, const Type *b){
    if(a->simple){
        log("Trying to apply function of type ");
        log_t(b);
        log("to a primitive value of type ");
        log_t(a);
        return NULL;
    }
    if(!equal_t(a->arg, b, a->gen)){
        log("Function of type ");
        log_t(b);
        log(" cannot be assigned to a function of type ");
        log_t(a);
        log(" in a context: ");
        log_context(a->gen);
        return NULL;
    }
    a->ret->gen = a->gen;
    return a->ret;
}


Parsed _parse_arg(const char input[]){
    Parsed res;
    input = skip_ws(input);
    if(*input == '('){
        return parse_t(input);
    }
    res.ret = malloc(sizeof (Type));
    res.ret->simple = true;
    char *name = calloc(20, sizeof (char));
    int i = 0;
    for(; (*input != '\0') && (*input != '\n');input++){
        char c = *input;
        if(c == ')' || c == ' ' || c == '-') {
            break;
        }
        if((c>='a' && c<='z') || (c>='A' && c<='Z')){
            name[i] = c;
            i++;
            continue;
        }
        free(name);
        log("Unexpected sybol \'%c\' in a function name", c);
        return (Parsed){NULL, NULL};
    }
    res.ret->name = name;
    res.left = input;
    if(generic(*res.ret))
        generics_add(res.ret, name);
    return res;
}
Parsed _parse_ret(const char input[]){
    input = skip_ws(input);
    if(*input == '-'){
        return parse_t(input+1);
    }
    if(*input == ')')
        return (Parsed){NULL, input + 1};
    return (Parsed){NULL, input};
}

Parsed parse_t(const char input[]){
    Parsed res;
    input = skip_ws(input);
    res.ret = malloc(sizeof (Type));
    Parsed arg, ret;
    switch(*input){
        case '(':
            arg = parse_t(input+1);
        break;
        default:
            arg = _parse_arg(input);
    }
    input = arg.left;
    res.ret->arg = arg.ret;
    res.ret->simple = false;
    generics_merge(res.ret, arg.ret);
    ret = _parse_ret(input);
    input = ret.left;
    if(!ret.ret){
        res.ret = arg.ret;
    } else {
        generics_merge(res.ret, ret.ret);
        res.ret->ret = ret.ret;
    }
    res.left = input;
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
