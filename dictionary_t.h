#include "types.h"
typedef struct dict{
    const struct dict * next;
    const Fun * value;
} dict;
dict* dict_add(const dict *d, const Fun *value);
const Fun* dict_get(const dict *d, const char *name);


#define list(type) struct {void *next; type *val;}

#define ignore_pt(code) \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wincompatible-pointer-types\"") \
        code; \
    _Pragma ("GCC diagnostic pop")

#define list_get(type, l, i, p) ignore_pt(p = l; for(int k = 0; k < i;k++) p = (list(type) *)p->next)

#define list_put(type, l, e) ignore_pt({list(type) *t = malloc(sizeof(list(type))); t->next = l; t->val = e; l = t;})
