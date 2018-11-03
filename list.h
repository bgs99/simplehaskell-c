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
#define list_add_last(name, dict, value) \
    { \
        if(!*dict){ \
            *dict = malloc(sizeof(name));\
            (*dict)->val = value; \
            (*dict)->next = NULL; \
        } else { \
            name *ret_list = malloc(sizeof(name)); \
            ret_list->val = value; \
            ret_list->next = NULL; \
            for(name *i = *dict; i; i = i->next) \
                if(!i->next){ \
                    i->next = ret_list; \
                    break; \
            } \
        } \
    }
