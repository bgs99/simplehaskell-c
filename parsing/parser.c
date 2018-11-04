#include "parser.h"
#include "freemem.h"
#include "type_parser.h"
#include "expression_parser.h"
#include "pattern_parser.h"

#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

struct syntax_tree undefined = {UNDEFINED, {NULL, 0}, NULL};

void skip_ws(const char **input){
    while(**input != '\n' && **input <= ' ' && **input)
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
    if(!accept_word(input, "import "))
        return ret;
    struct word module = read_word(input);
    if(!module.length){
        fprintf(stderr, "Expected module name after import directive \'!\'");
        return ret;
    }
    ret.type = IMPORT;
    ret.val = module;
    return ret;
}

struct syntax_tree accept_datatype(const char **input){
    struct syntax_tree ret = undefined;
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
            struct syntax_tree carg = accept_value_type(input);
            if(carg.type == UNDEFINED)
                break;
            struct syntax_tree *carg_p = malloc(sizeof (struct syntax_tree));
            *carg_p = carg;
            list_add_last(tree_args, &constr->args, carg_p);
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

struct syntax_tree accept_definition(const char **input){
    struct syntax_tree ret = undefined;
    const char *start = *input;
    struct word name = read_word(input);
    if(!name.length)
        return ret;
    char first = *name.begin;
    if(!(first >= 'a' && first <= 'z')){
        *input = start;
        return ret;
    }
    ret.val = name;
    while (true) {
        struct syntax_tree arg = accept_pattern(input);
        if(arg.type == UNDEFINED)
            break;
        struct syntax_tree *arg_p = malloc(sizeof (struct syntax_tree));
        *arg_p = arg;
        list_add_last(tree_args, &ret.args, arg_p);
    }
    if(!accept(input, '=')){
        *input = start;
        free_tree_args(ret.args);
        return ret;
    }
    struct syntax_tree *exp_p = malloc(sizeof (struct syntax_tree));
    mark_ptr(exp_p);
    *exp_p = accept_expression(input);
    if(exp_p->type == UNDEFINED){
        *input = start;
        free_tree_args(ret.args);
        free(exp_p);
        return ret;
    }
    list_add(tree_args, &ret.args, exp_p);
    ret.type = DEFINITION;
    return ret;
}

void skip_el(const char **input){
    if(!input)
        return;
    if(!*input)
        return;
    while (**input <= ' ' && **input) {
        (*input)++;
    }
}

struct syntax_tree accept_function(const char **input){
    skip_el(input);
    struct syntax_tree *ann = malloc(sizeof (struct syntax_tree));
    mark_ptr(ann);
    *ann = accept_annotation(input);
    if(ann->type == UNDEFINED)
        return *ann;
    struct syntax_tree ret = undefined;
    ret.val = ann->val;
    skip_el(input);
    while(true){
        const char *line = *input;

        struct syntax_tree pat = accept_definition(input);
        if(pat.type == UNDEFINED)
            break;
        if(pat.val.length != ret.val.length){
            *input = line;
            break;
        }
        if(strncmp(pat.val.begin, ret.val.begin, ret.val.length) != 0){
            *input = line;
            break;
        }
        struct syntax_tree *pat_p = malloc(sizeof (struct syntax_tree));
        *pat_p = pat;
        list_add_last(tree_args, &ret.args, pat_p);
        skip_el(input);
    }
    list_add(tree_args, &ret.args, ann);
    ret.type =FUNCTION;
    return ret;
}

struct syntax_tree accept_block(const char **input){
    skip_el(input);
    struct syntax_tree block;
    block = accept_import(input);
    if(block.type == UNDEFINED)
        block = accept_datatype(input);
    if(block.type == UNDEFINED)
        block = accept_function(input);

    return block;
}
