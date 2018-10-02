#include "types.h"
#include <malloc.h>
#include "reader.h"

bool equal_t(Type a, Type b){
    if(a.simple != b.simple) return false;//if a is function and b is val
    if(a.simple)
        return !strcmp(a.val.name, b.val.name);//if both val compare names

    if(!equal_t(*a.val.func.arg, *b.val.func.arg)) return false;//if args are different
    return equal_t(*a.val.func.ret, *b.val.func.ret);//comparing return types
}
const Type* apply_t(Type a, Type b){
    if(a.simple) return NULL;
    if(!equal_t(*a.val.func.arg, b)) return NULL;
    return a.val.func.ret;
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
    res.ret->val.name = name;
    res.left = input;
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
    res.ret->val.func.arg = arg.ret;
    res.ret->simple = false;
    ret = _parse_ret(input);
    input = ret.left;
    if(!ret.ret){
        res.ret = arg.ret;
    } else {
        res.ret->val.func.ret = ret.ret;
    }
    res.left = input;
    return res;
}

size_t print_t(const Type *t, char *s){
    if(t->simple){
        strcpy(s,t->val.name);
        return strlen(t->val.name);
    }
    char *origin = s;
    if(t->val.func.arg->simple){
        s += print_t(t->val.func.arg, s);
    } else {
        *s = '(';
        s++;
        s+= print_t(t->val.func.arg,s);
        *s=')';
        s++;
    }
    if(!t->val.func.ret) return (size_t)(s - origin);
    *s = '-';
    s++;
    return print_t(t->val.func.ret, s) + (size_t)(s - origin);
}


const Type* last_type(const Type *t){
    if(t->simple || !t->val.func.ret)
        return t;
    return last_type(t->val.func.ret);
}
