#include "parser.h"
#include "type_parser.h"

#include <stddef.h>
#include <stdio.h>
#include <malloc.h>

struct syntax_tree undefined = {UNDEFINED, {NULL, 0}, NULL};

void skip_ws(const char **input){
    while(**input <= ' ' && **input)
        (*input)++;
}
/**
 * @brief accept_word reads single word from input string. Word should contain only letters, digits and underscore
 * @param input input string
 * @return first word from input
 */
struct word read_word(const char **input){
    struct word ret = {NULL, 0};

    if(!input){
        fprintf(stderr, "Expected string pointer in read_word");
        return ret;
    }
    if(!*input){
        fprintf(stderr, "Expected string in read_word");
        return ret;
    }

    skip_ws(input);
    ret.begin = *input;
    while((**input >= '0' && **input <= '9') ||
          (**input >= 'A' && **input <= 'Z') ||
          (**input >= 'a' && **input <= 'z') ||
          **input == '_')
        (*input)++;
    ret.length = (unsigned long)(*input - ret.begin);
    return ret;
}
bool accept(const char **input, char c){
    skip_ws(input);
    if(**input == c){
        (*input)++;
        return true;
    }
    return false;
}
bool accept_word(const char **input, const char *word){
    skip_ws(input);
    int i = 0;
    for(; word[i]; i++){
        if(word[i] != (*input)[i])
            return false;
    }
    (*input)+=i;
    return true;
}

void free_tree_args(tree_args *args){
    if(!args)
        return;
    free_tree_args(args->next);
    free(args);
}

struct syntax_tree accept_import(const char **input){
    struct syntax_tree ret = undefined;
    if(!accept(input, '!'))
        return ret;
    struct word module = read_word(input);
    if(!module.length){
        fprintf(stderr, "Expected module name after import directive \'!\'");
        return ret;
    }
    ret.val = module;
    return ret;
}

struct syntax_tree accept_datatype(const char **input){
    struct syntax_tree ret = undefined;
    const char *start = *input;
    if(!accept_word(input, "data"))
        return ret;
    struct word name = read_word(input);
    char first = *name.begin;
    if(!(first >= 'A' && first <= 'Z')){
        fprintf(stderr, "Datatype name should start with a capital letter");
        return ret;
    }
    if(!accept(input, '=')){
        fprintf(stderr, "Expected '=' after datatype name");
        return ret;
    }
    ret.val = name;
    while(true){
        struct word name = read_word(input);
        if(!name.length)
            break;
        char first = *name.begin;
        if(!(first >= 'A' && first <= 'Z')){
            fprintf(stderr, "Constructor name should start with a capital letter");
            return ret;
        }
        struct syntax_tree *constr = malloc(sizeof (struct syntax_tree));
        constr->type = CONSTRUCTOR;
        constr->args = NULL;
        constr->val = name;
        while(true){
            struct syntax_tree carg = accept_fun_type(input);
            if(carg.type == UNDEFINED)
                break;
            struct syntax_tree *carg_p = malloc(sizeof (struct syntax_tree));
            *carg_p = carg;
            list_add(tree_args, &constr->args, carg_p);
        }
        accept(input, '|');
        list_add(tree_args, &ret.args, constr);
    }
    ret.type = ret.args ? DATATYPE : UNDEFINED;
    return ret;
}

struct syntax_tree accept_annotation(const char **input){
    const char *start = *input;
    struct syntax_tree ret = undefined;
    struct word name = read_word(input);
    if(!name.length)
        return ret;
    char c = *name.begin;
    if(!(c >='a' && c <= 'z')){
        fprintf(stderr, "Function name should start with a small letter");
        return ret;
    }
    if(!accept_word(input, "::")){
        *input  = start;
        return ret;
    }
    ret.val = name;
    struct syntax_tree type = accept_fun_type(input);
    if(type.type == UNDEFINED)
        return ret;
    struct syntax_tree *type_p = malloc(sizeof (struct syntax_tree));
    *type_p = type;
    list_add(tree_args, &ret.args, type_p);
    ret.type = ANNOTATION;
    return ret;
}
