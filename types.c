#include "types.h"
#include <malloc.h>
#include "reader.h"
#include "dictionary_t.h"

bool equal_t(const Type *a, const Type *b, generics *context){
    if(a->simple && generic(*a)){
        return generics_bind(context, a->name, b);
    }
    if(a->simple != b->simple) return false;//if a is function and b is val
    if(a->simple)
        return !strcmp(a->name, b->name);//if both val compare names

    if(!equal_t(a->arg, b->arg, context)) return false;//if args are different
    return equal_t(a->ret, b->ret, context);//comparing return types
}

const Type* apply_t(const Type *a, const Type *b){
    if(a->simple) return NULL;
    if(!equal_t(a->arg, b, a->gen)) return NULL;
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
    char *name = malloc(sizeof (char)*20);
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

size_t print_t(const Type *t, char *s){
    if(t->simple){
        strcpy(s,t->name);
        return strlen(t->name);
    }
    char *origin = s;
    if(t->arg->simple){
        s += print_t(t->arg, s);
    } else {
        *s = '(';
        s++;
        s+= print_t(t->arg,s);
        *s=')';
        s++;
    }
    if(!t->ret) return (size_t)(s - origin);
    *s = '-';
    s++;
    return print_t(t->ret, s) + (size_t)(s - origin);
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
