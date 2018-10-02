#include <stdio.h>
#include "malloc.h"
#include "parser.h"



void put_t(const Type *t){
    char buff[20];
    print_t(t,buff);
    printf("%s\n",buff);
}


int main()
{
    const dict *d = init();
    const char *ip = "idn : Nat-Nat \n idn x = s x";
    parse_res fin = parse_fun(d, ip);
    //char buff[256];
    put_t(fin.val->type);
    printf("%s",fin.val->def);
    return 0;
}
