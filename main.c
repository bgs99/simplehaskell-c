#include <stdio.h>
#include "malloc.h"
#include "parser.h"


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
    char *all = calloc(len+1, sizeof (char));
    rewind(in);
    fread(all, 1, len, in);
    fclose(in);

    const dict *ad = parse_all(all);
    printf("%s \n> ", all);

    char *ui = calloc(200, sizeof (char));

    while(fgets(ui, 200, stdin)){

        printf("Expression %s\n Evaluates to ", ui);
        print_res(*eval_string(ad,ui));
        printf("\n\n> ");
    }

    return 0;
}
