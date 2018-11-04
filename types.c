#include "types.h"
#include <malloc.h>
#include "parsing/parser.h"
#include "dictionary_t.h"
#include "eval.h"
#include <string.h>

bool name_equal(struct word a, struct word b){
    if(a.length != b.length)
        return false;
    return strncmp(a.begin, b.begin, a.length) == 0;
}

/**
 * @brief Checks for type equality
 * @param a First type
 * @param b Second type
 * @param context Generics context
 * @return return true if types are equal or type a can be substitued with type b
 */
bool equal_t(Type *a, Type *b, generics *context){
    if(a->simple && generic(*a)){
        if(b->simple && generic(*b) && name_equal(a->name, b->name))
            return true;
        return generics_bind(context, a->name, b);
    }
    if(a->simple != b->simple)
        return false;//if a is function and b is val
    if(a->simple)
        return name_equal(a->name, b->name);//if both val compare names

    if(!equal_t(a->arg, b->arg, context))
        return false;//if args are different
    return equal_t(a->ret, b->ret, context);//comparing return types
}
Type *parse_t(struct syntax_tree input);
/**
 * @brief Calculates type of the application of function
 * @param a Type of a function
 * @param b Type of an argument
 * @return Result type if appliable, NULL otherwise
 */
Type* apply_t(const Type *a, Type *b){
    if(a->simple){
        fprintf(stderr, "Trying to apply function of type ");
        log_t(b);
        fprintf(stderr, "to a primitive value of type ");
        log_t(a);
        fprintf(stderr, "\n");
        return NULL;
    }
    if(!equal_t(a->arg, b, a->gen)){
        fprintf(stderr, "Function of type ");
        log_t(b);
        fprintf(stderr, " cannot be passed to a function of type ");
        log_t(a);
        fprintf(stderr, " in a context: ");
        log_context(a->gen);
        fprintf(stderr, "\n");
        return NULL;
    }
    a->ret->gen = a->gen;
    return a->ret;
}

/**
 * @brief Parses Type of a function's argument
 * @param input Syntax tree of a type
 * @return Parse result
 */
Type *_parse_arg(struct syntax_tree input){
    Type *res;
    if(input.type == FUN_TYPE){
        return parse_t(input);
    }
    res = calloc(1, sizeof (Type));
    res->simple = true;
    const struct word name = input.val;
    res->name = name;
    if(generic(*res))
        generics_add(res, name);
    return res;
}
/**
 * @brief Parses type of a function
 * @param input Syntax tree of a function's type
 * @return Result type
 */
Type *parse_t(struct syntax_tree input){
    if(input.type == VALUE_TYPE)
        return _parse_arg(input);
    Type *res = calloc(1, sizeof (Type));
    Type *arg, *ret;

    arg = _parse_arg(*input.args->val);

    res->arg = arg;
    res->simple = false;

    generics_merge(res, arg);

    ret = parse_t(*input.args->next->val);

    if(!ret){
        res = arg;
    } else {
        generics_merge(res, ret);
        res->ret = ret;
    }
    return res;
}
/**
 * @brief Prints type to a file
 * @param t Type
 * @param dest File
 */
void fprint_t(const Type *t, FILE *dest){
    if(!t){
        fprintf(dest, "No type provided\n");
        return;
    }
    if(t->simple){
        fprintf(dest, "%.*s", (int)t->name.length, t->name.begin);
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
    fprintf(dest, " -> ");
    fprint_t(t->ret, dest);
}

/**
 * @brief Gets final return type of a function
 * @param t Type of a function
 * @return Result type
 */
Type* last_type(Type *t){
    if(t->simple || !t->ret)
        return t;
    return last_type(t->ret);
}

/**
 * @brief Checks if type is generic
 * @param t Type
 * @return true if generic, false otherwise
 */
bool generic(Type t){
    if(!t.simple) return false;
    char c = *t.name.begin;
    return (c >= 'a') && (c <= 'z');
}
/**
 * @brief Prints generics context to a file
 * @param g Generics context
 * @param f File
 */
void fprint_context(generics *g, FILE *f){
    if(!g) {
        fprintf(f, "No type vars\n");
        return;
    }
    for(generics *i = g; i; i = i->next){
        fprintf(f, "\t%.*s = ",(int)g->key.length, g->key.begin);
        fprint_t(g->val, f);
        fprintf(f, "\n");
    }
}

/**
 * @brief Creates type from a name
 * @param name Name
 * @return Result type
 */
Type* type_make(struct word name){
    Type *ret = calloc(1, sizeof (Type));
    ret->simple = true;
    ret->name = name;
    if(generic(*ret))
        generics_add(ret, name);
    return ret;
}

/**
 * @brief Adds return type to a function's type
 * @param fun Function's type
 * @param arg Return type
 * @return
 */
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

/**
 * @brief Checks if two objects are equal
 * @param a First object
 * @param b Second object
 * @return true if equal, false otherwise
 */
bool object_equal(object a, object b){
    if(a.argc != b.argc)
        return false;
    if(!name_equal(a.name, b.name))
        return false;
    for(int i = 0; i < a.argc; i++){
        object x = *promise_eval(a.args + i);
        object y = *promise_eval(b.args + i);
        if(!object_equal(x, y))
            return false;
    }
    return true;
}

/**
 * @brief Resets generics binding of a type
 * @param t Type
 */
void reset_generics(Type *t){
    for(generics *i = t->gen; i; i = i->next){
        i->val = NULL;
    }
}
