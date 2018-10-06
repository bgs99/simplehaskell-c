#include "reader.h"
void skip_ws(const char **input){
    while(**input == ' ' || **input == '\t')
        (*input)++;
}
void read_word(char *buff, const char **input){
    for(int i = 0;;(*input)++, i++){
        const char c = **input;
        if((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9') || c=='.'){
            buff[i] = c;
            continue;
        }
        break;
    }
}
