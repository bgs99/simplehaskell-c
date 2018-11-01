#include <stdio.h>
#include "malloc.h"
#include "eval.h"
#include "parsing/parser.h"
#include "parsing/pattern_parser.h"

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

    /*const dict *ad = parse_all(all);
    printf("%s \n> ", all);

    char *ui = calloc(200, sizeof (char));

    while(fgets(ui, 200, stdin)){
        if(*ui == '\n'){
            printf("> ");
            continue;
        }
        printf("Expression %s\n Evaluates to ", ui);
        print_res(*eval_string(ad,ui));
        printf("\n\n> ");
    }
    */
    //const char *input = "length (List _ tail) = S (length tail)";
    struct syntax_tree res = accept_program(&all);
    puts("");
    free_tree_args(res.args);
    return 0;
}
