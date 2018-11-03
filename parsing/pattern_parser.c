#include "pattern_parser.h"
#include <stdio.h>
#include <malloc.h>

struct syntax_tree accept_pattern(const char **input){
    struct syntax_tree ret = undefined;
    const char *start = *input;
    bool par = accept(input, '(');
    struct word name = read_word(input);
    if(!name.length)
        return ret;
    ret.val = name;
    if(par){
        char first = *name.begin;
        if(!(first >= 'A' && first <= 'Z')){
            *input = start;
            return ret;
        }
        while(true){
            struct syntax_tree arg = accept_pattern(input);
            if(arg.type == UNDEFINED)
                break;
            struct syntax_tree *arg_p = malloc(sizeof (struct syntax_tree));
            *arg_p = arg;
            list_add_last(tree_args, &ret.args, arg_p);
        }
        if(!accept(input, ')')){
            fprintf(stderr, "Expected ')' after '(' in pattern match");
            free_tree_args(ret.args);
            return ret;
        }
        ret.type = ret.args ? COMPLEX_ARG : VAL;
        return ret;
    }
    ret.type = (*name.begin <='Z') ? VAL : VAR;
    return ret;
}
