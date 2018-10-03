#include <stdio.h>
#include "malloc.h"
#include "parser.h"


void put_t(const Type *t){
    char buff[20];
    print_t(t,buff);
    printf("%s\n",buff);
}
void put_f(const Fun *f){
    char buff[20];
    if(print_res(*f,buff))
        printf("%s\n",buff);
    else printf("function\n");
}

int main(int argc, char **argv)
{
    const char *path = "input.shs";
    if(argc > 1)
        path = argv[1];
    FILE *in = fopen(path,"r");
    if(!in){
        printf("Cannot open file %s, halting", path);
        return -1;
    }
    fseek(in, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(in);
    char *all = calloc(len, sizeof (char));
    rewind(in);
    fread(all, 1, len, in);
    fclose(in);

    const dict *ad = parse_all(all);
    printf("%s \n", all);
    const char *ip = "const z z";
    printf("Expression %s\n Evaluates to ", ip);
    put_f(eval_string(ad,ip));
    return 0;
}
