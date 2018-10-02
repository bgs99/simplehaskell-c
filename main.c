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

int main()
{
    const char *all = "cf:Nat-Nat\ncf x = s (s x)\n g:Nat\n g = s (cf z)";

    const dict *ad = parse_all(all);

    printf("%s \n", all);
    const char *ip = "p g g";
    printf("Expression %s\n Evaluates to ", ip);
    put_f(eval_string(ad,ip));
    return 0;
}
