#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <malloc.h>

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
    if(!**input){
        fprintf(stderr, "Expected word after whitespaces in read_word");
        return ret;
    }
    ret.begin = *input;
    while((**input >= '0' && **input <= '9') ||
          (**input >= 'A' && **input <= 'Z') ||
          (**input >= 'a' && **input <= 'z') ||
          **input == '_')
        (*input)++;
    ret.length = *input - ret.begin;
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

static struct syntax_tree undefined = {UNDEFINED, {NULL, 0}, NULL};

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

void free_tree_args(tree_args *args){
    if(!args)
        return;
    free_tree_args(args->next);
    free(args);
}

struct syntax_tree accept_type(const char **input);

struct syntax_tree accept_value_type(const char **input){
    struct syntax_tree ret = undefined;
    struct word name = read_word(input);
    if(!name.length)
        return ret;
    char first = *name.begin;
    if(!(first >= 'A' && first <= 'Z'))
        return ret;
    ret.type = VALUE_TYPE;
    ret.val = name;
    return ret;
}

struct syntax_tree accept_complex_type(const char **input){
    struct syntax_tree ret = undefined;
    struct syntax_tree name = accept_value_type(input);
    if(name.type == UNDEFINED)
        return ret;
    ret.val = name.val;
    ret.args = NULL;
    while(true){
        struct syntax_tree arg = accept_fun_type(input);
        if(arg.type == UNDEFINED){
            break;
        }
        struct syntax_tree *arg_p = malloc(sizeof (struct syntax_tree));
        *arg_p = arg;
        list_add(tree_args, &ret.args, arg_p);
    }
    ret.type = ret.args ? COMPLEX_TYPE : VALUE_TYPE;
    return ret;
}

struct syntax_tree accept_fun_type(const char **input){
    struct syntax_tree ret = undefined;
    const char *start = *input;
    struct syntax_tree head = undefined;
    if(accept(input, '(')){
        head = accept_fun_type(input);
        if(!accept(input, ')')){
            fprintf(stderr, "Expected ')' after '(' in type");
            return ret;
        }
    } else {
        head = accept_complex_type(input);
    }

    if(head.type == UNDEFINED){
        return ret;
    }


    if(!accept_word(input, "->")){
        return head;
    }
    struct syntax_tree tail = accept_fun_type(input);
    if(tail.type == UNDEFINED){
        free_tree_args(head.args);
        *input = start;
        return ret;
    }
    struct syntax_tree *head_p = malloc(sizeof (struct syntax_tree)),
            *tail_p = malloc(sizeof (struct syntax_tree));
    *head_p = head;
    *tail_p = tail;
    list_add(tree_args, &ret.args, head_p);
    list_add(tree_args, &ret.args, tail_p);
    ret.type = FUN_TYPE;
    return ret;
}



