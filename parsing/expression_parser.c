#include "expression_parser.h"
#include <malloc.h>

struct syntax_tree accept_expression(const char **input){
    struct syntax_tree ret = undefined;
    if(accept(input, ')'))
        return ret;
    struct word name = read_word(input);
    if(!name.length)
        return ret;
    ret.val = name;
    ret.type = EXPRESSION;
    while (true) {
        struct syntax_tree arg = accept_expression(input);
        if(ret.type == UNDEFINED)
            break;
        struct syntax_tree *arg_p = malloc(sizeof (struct syntax_tree));
        *arg_p = arg;
        list_add(tree_args, &ret.args, arg_p);
    }
    return ret;
}
