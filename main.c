#include <stdio.h>
#include "malloc.h"
#include "eval.h"
#include "processing/process.h"
#include <readline/readline.h>
#include <readline/history.h>

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

    const dict *ad = process_all(all);
    printf("%s \n", all);

    char *ui;

    while((ui = readline("> "))!=NULL){
        if(*ui == '\0'){
            continue;
        }
        printf("Expression %s\n Evaluates to ", ui);
        print_res(*eval_string(ad,ui));
        printf("\n");
        add_history(ui);
        free(ui);
    }

    return 0;
}
