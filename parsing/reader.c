#include "reader.h"
void skip_ws(const char **input){
    while(**input == ' ' || **input == '\t')
        (*input)++;
}
void read_word(char *buff, const char **input){
    for(int i = 0;;(*input)++, i++){
        const char c = **input;
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
            if(buff)
                buff[i] = c;
            continue;
        }
        break;
    }
}

bool is_const(const char *input){
    char c = *input;
    return (c >= '0' && c <='9') || (c >= 'A' && c <= 'Z');
}

void skip_el(const char **input){
    while(**input == '\n' || **input == ')' || **input == ' ' || **input == '\t')
        (*input)++;
}
