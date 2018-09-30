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
    /*const dict *d = init();
    const Fun *tf = dict_get(d, "s");
    const Fun *ta = dict_get(d, "z");

    printf("Function %s: ", tf->name);
    put_t(tf->type);

    printf("Applied to ");
    put_t(ta->type);
    printf("With value %d\n", ta->val.i_val);
    printf("Has result ");

    char buff[20];
    apply(d,"s","z",buff);
    printf("%s\n",buff);
*/
    const char *ip = "idn : Nat-Nat\nidn x = x";
    parse_res fin = parse_fun(ip);
    //char buff[256];
    put_t(fin.val->type);
    printf("%s",fin.val->def);
    return 0;
}
