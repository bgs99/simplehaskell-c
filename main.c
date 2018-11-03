#include <stdio.h>
#include "malloc.h"
#include "eval.h"
#include "processing/process.h"
#include <readline/readline.h>
#include <readline/history.h>

static dict *ad = NULL;
static const char **names;
char * function_name_generator(const char *text, int state);

char **function_completion(const char *text, int start, int end){
    rl_attempted_completion_over = 1;
    end = start;
    return rl_completion_matches(text, function_name_generator);
}


char *function_name_generator(const char *text, int state){

    static unsigned long list_index, len;
    const char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = names[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
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
    char *all = calloc(len+1, sizeof (char));
    rewind(in);
    fread(all, 1, len, in);
    fclose(in);

    ad = process_all(all);

    unsigned long adl = 0;
    for(dict *td = ad; td; td = td->next)
        adl++;
    names = calloc(adl+1, sizeof (char *));
    int i = 0;
    for(dict *td = ad; td; td = td->next, i++){
        names[i] = td->val->val->t->f->name;
    }


    rl_attempted_completion_function = function_completion;

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
