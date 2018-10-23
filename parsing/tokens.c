#include "tokens.h"
#include "malloc.h"

char* get_name(token_list **tl){
    char *name = calloc((*tl)->val->length+1, sizeof (char));
    strncpy(name, (*tl)->val->begin, (*tl)->val->length);
    *tl = (*tl)->next;
    return name;
}
