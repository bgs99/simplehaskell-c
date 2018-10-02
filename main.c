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
    print_res(*f,buff);
    printf("%s\n",buff);
}

int main()
{
    const dict *d = init();
    const char *cf = "cf : Nat - Nat \ncf x = s (s x)";
    printf("%s \n", cf);
    const char *ip = "s (cf z)";
    const eval_tree *cfet = parse_fun(d,cf).et;
    d = dict_add_eval(d, cfet);
    printf("Expression %s\n Evaluates to ", ip);
    put_f(eval_string(d,ip));
    //parse_res fin = parse_fun(d, ip);
    //char buff[256];
    //put_t(fin.et->f->type);
    return 0;
}
