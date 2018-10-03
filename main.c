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
    const char *all =
        "plus_two : Nat - Nat \n \
        plus_two x = s (s x) \n \
        three : Nat \n \
        three = s (plus_two z) \n \
        mkeq : Nat - Nat - Nat \n \
        mkeq x y = if (equal x y) x (mkeq (s x) y)";

    const dict *ad = parse_all(all);
    printf("%s \n", all);
    const char *ip = "if true z (s z)";
    printf("Expression %s\n Evaluates to ", ip);
    put_f(eval_string(ad,ip));
    return 0;
}
