#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

struct syntax_tree accept_fun_type(const char **input);

struct syntax_tree accept_value_type(const char **input){
    struct syntax_tree ret = undefined;
    struct word name = read_word(input);
    if(!name.length)
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
    while(true){
        struct syntax_tree arg = accept_fun_type(input);
        if(arg.type == UNDEFINED){
            break;
        }
        struct syntax_tree *arg_p = malloc(sizeof (struct syntax_tree));
        *arg_p = arg;
        list_add(tree_args, &ret.args, arg_p);
    }
    if(ret.args){
        char first = *name.val.begin;
        if(!(first >= 'A' && first <= 'Z')){
            free_tree_args(ret.args);
            return ret;
        }
        ret.type = COMPLEX_TYPE;
        return ret;
    }
    return name;
}

void print_type(const struct syntax_tree tree){
    char *name;
    switch(tree.type){
        case FUN_TYPE:
            putchar('(');
            print_type(*tree.args->next->val);
            printf(" -> ");
            print_type(*tree.args->val);
            putchar(')');
            break;
        case COMPLEX_TYPE:
            putchar('(');
            name = calloc(tree.val.length+1, 1);
            strncpy(name,tree.val.begin,tree.val.length);
            printf("%s", name);
            for(tree_args *i = tree.args; i; i = i->next){
                putchar(' ');
                print_type(*i->val);
            }
            putchar(')');
            break;
        case VALUE_TYPE:
            name = calloc(tree.val.length+1, 1);
            strncpy(name,tree.val.begin,tree.val.length);
            printf("%s", name);
            break;
        default:
            break;
    }
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
    list_add(tree_args, &ret.args, tail_p);
    list_add(tree_args, &ret.args, head_p);
    ret.type = FUN_TYPE;
    return ret;
}



