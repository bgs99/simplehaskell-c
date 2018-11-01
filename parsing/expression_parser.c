#include "expression_parser.h"
#include <malloc.h>

struct syntax_tree accept_expression(const char **input){
    struct syntax_tree ret = undefined;
    struct word name = read_word(input);
    if(!name.length)
        return ret;
    ret.val = name;
    while (true) {
        struct syntax_tree arg = undefined;
        if(accept(input, '(')){
            arg = accept_expression(input);
            if(!accept(input, ')')){
                fprintf(stderr, "Expected ')' after '(' in expression");
                free_tree_args(ret.args);
                return ret;
            }
        } else {
            arg.val = read_word(input);
            arg.type = EXPRESSION;
            if(!arg.val.length)
                break;
        }
        if(arg.type == UNDEFINED)
            break;
        struct syntax_tree *arg_p = malloc(sizeof (struct syntax_tree));
        *arg_p = arg;
        list_add(tree_args, &ret.args, arg_p);
    }
    ret.type = EXPRESSION;
    return ret;
}
