#include "eval.h"
#include "stdio.h"

Fun apply_v(const Fun a, const Fun b){
    return (Fun){
        .name = NULL,
        .val = a.val.f_val(b.val),
        .type= apply_t(*a.type, *b.type)
    };
}

bool print_res(const Fun f, char* buff){
    if(!f.type->simple) return false;
    if(!strcmp(f.type->val.name, "Nat")){
        sprintf(buff, "%d", f.val.i_val);
    } else if(!strcmp(f.type->val.name, "Double")){
        sprintf(buff, "%f", f.val.d_val);
    } else if(!strcmp(f.type->val.name, "Bool")){
        if(f.val.b_val)
            sprintf(buff, "true");
        else sprintf(buff, "false");
    } else if(!strcmp(f.type->val.name, "Char")){
        sprintf(buff, "%c", f.val.c_val);
    }
    return true;
}

bool apply_f(const Fun a, const Fun b, char* buff){
    Fun res = apply_v(a, b);
    if(!res.type){
        return false;
    }
    buff+=print_t(res.type, buff);
    *buff = '\n';
    buff++;
    if(!print_res(res,buff)){
        sprintf(buff, "Cannot print a function");
    }
    return true;
}

bool apply(const dict *d, const char * const a, const char * const b, char* buff){
    const Fun *fa = dict_get(d, a);
    const Fun *fb = dict_get(d, b);
    if(!fa || !fb){
        return false;
    }
    return apply_f(*fa,*fb, buff);
}
