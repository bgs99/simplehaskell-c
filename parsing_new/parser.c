#include "parsing_new/parser.h"
#include <stddef.h>
#include <stdio.h>


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
    if(**input == c){
        (*input)++;
        return true;
    }
    return false;
}
