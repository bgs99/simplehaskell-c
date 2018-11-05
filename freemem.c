#include "freemem.h"
#include "eval.h"
#include "malloc.h"

static struct memlog{
    void *ptr;
    struct memlog *next;
} *log = NULL;

void mark_ptr(void *ptr){
    if(!ptr)
        return;
    for(struct memlog *i = log; i; i = i->next){
        if((unsigned long)i->ptr == (unsigned long)ptr)
            return;
    }
    struct memlog *new = malloc(sizeof (struct memlog));
    new->next = log;
    new->ptr = ptr;
    log = new;
}

void free_log(){
    while(log){
        struct memlog *ml = log;
        log = log->next;
        free(ml->ptr);
        ml->ptr = NULL;
        ml->next = NULL;
        free(ml);
    }
}


void eval_tree_free(struct eval_tree *t);
void object_free(struct object *o);
void type_free(struct Type *t);

void promise_free(struct eval_promise *p){
    if(!p)
        return;
    dict_free(p->glob);
    mark_ptr(p->glob);
    p->glob = NULL;
    eval_tree_free(p->input);
    mark_ptr(p->input);
    p->input = NULL;
    object_free(p->val);
    mark_ptr(p->val);
    p->val = NULL;
    for(unsigned i = 0; i < p->paramc; i++){
        promise_free(p->params + i);
        mark_ptr(p->params + i);
    }
    p->paramc = 0;
}

void generics_free(struct generics *g){
    if(!g)
        return;
    generics_free(g->next);
    mark_ptr(g->next);
    g->next = NULL;
    type_free(g->val);
    mark_ptr(g->val);
    g->val = NULL;
}

void object_free(struct object *o){
    if(!o)
        return;
    for(int i = 0; i < o->argc; i++){
        promise_free(o->args + i);
        mark_ptr(o->args + i);
    }
}

void type_free(struct Type *t){
    if(!t)
        return;
    if(t->simple)
        return;
    generics_free(t->gen);
    mark_ptr(t->gen);
    t->gen = NULL;
    type_free(t->arg);
    mark_ptr(t->arg);
    t->arg = NULL;
    type_free(t->ret);
    mark_ptr(t->ret);
    t->ret = NULL;
}

void function_free(struct Fun *f){
    if(!f)
        return;
    mark_ptr(f->ids);
    f->ids = NULL;
    type_free(f->type);
    mark_ptr(f->type);
    f->type = NULL;
    object_free(f->val);
    mark_ptr(f->val);
    f->val = NULL;
}

void arg_free(struct arg *a){
    if(!a)
        return;
    function_free(a->match);
    mark_ptr(a->match);
    a->match = NULL;
    if(a->complex){
        for(arg_list *i = a->args; i; i = i->next){
            arg_free(i->val);
            mark_ptr(i->val);
            i->val = NULL;
        }
        mark_ptr(a->args);
        a->args = NULL;
    }
}

void eval_tree_free(struct eval_tree *t){
    if(!t)
        return;
    if(!t->f)
        return;
    t->argn = t->argn;
    t->next = NULL;
    function_free(t->f);
    mark_ptr(t->f);
    t->f = NULL;
    for(int i = 0; (i < -t->argn) && t->arg; i++){
        eval_tree_free(t->arg + i);
        mark_ptr(t->arg + i);
    }
}

void arg_list_free(arg_list *al){
    if(!al)
        return;
    arg_list *next = al->next;
    al->next = NULL;
    arg_list_free(next);

    arg_free(al->val);
    mark_ptr(al->val);
    al->val = NULL;
    mark_ptr(al);
}

void pattern_free(struct pattern *p){
    if(!p)
        return;
    eval_tree_free(p->t);
    mark_ptr(p->t);
    p->t = NULL;
    arg_list_free(p->args);
    p->args = NULL;
}

void pattern_list_free(pattern_list *pl){
    if(!pl)
        return;
    for(pattern_list *i = pl; i; i = i->next){
        pattern_free(i->val);
        mark_ptr(i->val);
        i->val = NULL;
    }
}

void dict_free(dict *d){
    if(!d)
        return;
    for(dict *i = d; i; i = i->next){
        pattern_list_free(i->val);
        mark_ptr(i->val);
        i->val = NULL;
    }
}



void free_all(dict *d){
    dict_free(d);
    free_log();
    free(log);
    log = NULL;
}

void syntax_tree_free(struct syntax_tree tree){
    for(tree_args *ta = tree.args; ta; ta = ta->next){
        syntax_tree_free(*ta->val);
        mark_ptr(ta->val);
        mark_ptr(ta);
    }
}
