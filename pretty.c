#include "pretty.h"
#include "eval.h"

unsigned get_nat(object val){
    if(*val.name.begin == 'Z'){
        return 0;
    }
    return get_nat(*promise_eval(val.args))+1;
}

void print_list(object val){
    if(*val.name.begin == 'N'){//for Nil
        printf("[]");
        return;
    }
    printf("[%d", get_nat(*promise_eval(val.args)));
    for(object *i = promise_eval(val.args+1);*i->name.begin != 'N'; i = promise_eval(i->args+1)){
        printf(", %d", get_nat(*promise_eval(i->args)));
    }
    printf("]");
    fflush(stdout);
}
