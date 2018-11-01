#pragma once
#define define_list(type, name) \
    typedef struct name{ \
        type *val; \
        struct name *next;\
    } name;
#define list_create(name, val) (name) {val, NULL}
#define list_add(name, dict, value) \
    { \
        name *ret_list = malloc(sizeof(name)); \
        ret_list->val = value; \
        ret_list->next = *dict; \
        *dict = ret_list; \
    }
