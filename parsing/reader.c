#include "reader.h"


bool is_const(const char *input){
    char c = *input;
    return (c >= '0' && c <='9') || (c >= 'A' && c <= 'Z');
}

void skip_el(const char **input){
    while(**input == '\n' || **input == ')' || **input == ' ' || **input == '\t')
        (*input)++;
}
