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
    const char *all = "plus_two:Nat-Nat\nplus_two x = s (s x)\n three:Nat\nthree = s (plus_two z)";

    const dict *ad = parse_all(all);

    printf("%s \n", all);
    const char *ip = "if true (plus three three) z";
    printf("Expression %s\n Evaluates to ", ip);
    put_f(eval_string(ad,ip));
    return 0;
}
