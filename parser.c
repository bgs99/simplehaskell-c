#include "parser.h"
#include "reader.h"
#include "eval.h"
#include <malloc.h>


const char* get_name(const token_list **tl){
    char *name = calloc((*tl)->len+1, sizeof (char));
    strncpy(name, (*tl)->begin, (*tl)->len);
    *tl = (*tl)->next;
    return name;
}

token_list* reverse(token_list *input, token_list **end){
    if(!input) return NULL;
    if(!input->next) {
        *end = input;
        return input;
    }
    token_list *ep = NULL;
    token_list *revd = reverse(input->next, &ep);
    input->next = NULL;
    ep->next = input;
    *end = input;
    return revd;
}

/**
 * @brief Splits input into tokens, should only be used on right side of assignment
 * @param input string
 * @return tokenized input
 */

token_list* tokenize_r(const char **input){
    token_list *ret = NULL;
    while(**input != '\0' || **input != '\n'){
        skip_ws(input);
        token_list *new;
        switch (**input) {
            case '\0':
            case '\n':
                return ret;
            case '(':
            case ')':
            case '-':
            case '=':
            case ':':
                new = malloc(sizeof(token_list));
                new->len = 1;
                new->begin = (*input)++;
                new->next = ret;
                ret = new;
                break;
            default:
                new = malloc(sizeof (token_list));
                new->begin = *input;
                read_word(NULL,input);
                new->len = (unsigned)(*input - new->begin);
                new->next = ret;
                ret = new;
        }
    }
    return ret;
}


token_list* tokenize(const char **input){
    token_list *ret = tokenize_r(input);
    (*input)++;
    if(!ret->next) return ret;
    token_list *end = NULL;
    return reverse(ret, &end);
}

const Fun* parse_num(const char *str){
    Fun *f = malloc(sizeof (Fun));
    f->type = malloc(sizeof (Type));
    Prim *res = malloc(sizeof (Prim));
    Type *t = malloc(sizeof (Type));
    f->val = res;
    f->type = t;
    int sign = 1;
    if(*str == '-'){
        str ++;
        sign = -1;
    }
    unsigned long val = 0;
    int incdiv = 0;
    int div = 0;

    for(const char *i = str; *i !='\0' ; i++, div+=incdiv){
        if(*i == '.'){
            incdiv = 1;
            continue;
        }
        val *= 10;
        val += (unsigned)(*i - '0');
    }
    if(incdiv == 0){
        res->i_val = (int)val * sign;
        t->simple=true;
        t->name = alloc_name("Int");
        return f;
    }
    double nval = val;
    div--;
    for(;div > 0;div--)
        nval/=10;

    res->d_val = nval*sign;
    t->simple=true;
    t->name = alloc_name("Float");
    return f;
}

const Fun* get_fun(const dict *glob, const dict *local, const char *name){
    char c = *name;
    if(('0' <= c && '9' >= c) || c == '-'){
        return parse_num(name);
    }

    const Fun *ret = dict_get(local, name);
    if(!ret)
        ret = dict_get(glob, name);
    if(!ret){
        log("Function \"%s\" is not found in global dictionary and arguments", name);
        return NULL;
    }
    return ret;
}

pattern* pattern_from_et(eval_tree *et){
    pattern *p = calloc(1, sizeof (pattern));
    *p = (pattern){et, NULL, NULL};
    return p;
}


parse_res parse_tan(const token_list **input){
    if(!*input) return (parse_res){NULL,NULL};
    const char *name = get_name(input);
    if(*(*input)->begin != ':'){
        log("Expected \':\' in type annotation of %s", name);
        return (parse_res){NULL,NULL};
    }
    *input = (*input)->next;
    Fun *res = malloc(sizeof (Fun));
    res->name = name;
    Parsed p = parse_t(input);
    res->type = p.ret;
    eval_tree *et = eval_make(res);
    (*input)++;
    return (parse_res){NULL, pattern_from_et(et)};
}
void parse_left(const Fun *f,const dict **local, const token_list **input){
    if(!*input) return;
    const char *name = get_name(input);
    if(strcmp(name, f->name) != 0){
        log("Annotation's name \"%s\" and definition name \"%s\" do not match", name, f->name);
    }
    const Type *i = f->type;//--
    unsigned int lid = 1;
    while(*(*input)->begin != '='){
        const char *id = get_name(input);
        Fun *af = malloc(sizeof (Fun));
        af->name = id;
        //const Type *t = i->simple ? i : i->arg;
        af->type = i->arg;

        i = i->ret;

        af->lid = lid++;
        dict_add(local, af);

    }
    *input = (*input)->next;
}

parse_res parse_arg(const dict *local, const dict *glob, const token_list **input);
parse_res parse_f_f(const dict *local, const dict *glob, const token_list **input);

parse_res parse_app(const dict *local, const dict *glob, const token_list **input){
    if(!*input) return (parse_res){NULL,NULL};
    parse_res pr = parse_f_f(local, glob, input);
    const Type *f = pr.type;
    eval_tree *ret = pr.et->t;
    while((pr = parse_arg(local, glob, input)).type){

        f = apply_t(f, pr.type);
#ifdef LOGALL
        log("&&logging context\n");
        log_context(f->gen);
        log("\n");
#endif
        eval_add_arg(ret, pr.et->t);
    }
    return (parse_res){f, pattern_from_et(ret)};
}
parse_res parse_f_f(const dict *local, const dict *glob, const token_list **input){
    if(!*input) return (parse_res){NULL,NULL};
    if(*(*input)->begin == ')' || *(*input)->begin == '\n' || *(*input)->begin == '\0')
        return (parse_res){NULL, NULL};
    if(*(*input)->begin == '('){
        *input = (*input)->next;
        parse_res ret = parse_app(local, glob, input);
        if(*(*input)->begin == ')') *input = (*input)->next;
        return ret;
    }
    const char *name = get_name(input);

    const Fun *ff = get_fun(glob, local, name);
    return (parse_res){ff->type, pattern_from_et(eval_make(ff))};
}

parse_res parse_arg(const dict *local, const dict *glob, const token_list **input){
    if(!*input) return (parse_res){NULL,NULL};
    if(*(*input)->begin == ')' || *(*input)->begin == '\n' || *(*input)->begin == '\0')
        return (parse_res){NULL, NULL};
    if(*(*input)->begin == '('){
        *input = (*input)->next;
        parse_res ret = parse_app(local, glob, input);
        if(*(*input)->begin == ')')
            *input = (*input)->next;
        return ret;
    }
    const char *name = get_name(input);
    const Fun *ff = get_fun(glob, local, name);
    return (parse_res){ff->type, pattern_from_et(eval_make(ff))};
}


parse_res parse_right(const Type *f, const dict *local, const dict *glob, const token_list **input){
    if(!*input) return (parse_res){NULL,NULL};
    parse_res tr = parse_app(local, glob, input);
    if(!equal_t(last_type(f), generics_sub(tr.type, tr.type->gen), tr.type->gen)){
        log("Return types do not match: \n %s has type ", f->name);
        log_t(last_type(f));
        log("\n%s has type ", tr.et->t->f->name);
        log_t(last_type(tr.type));
        log(" in a context: \n");
        log_context(tr.type->gen);
        log("\n");
        return (parse_res){NULL, NULL};
    }
    return tr;
}

void pattern_add(parse_res *pr, parse_res np){
    if(!pr->et){
        pr->et = np.et;
        return;
    }

    pattern *i;
    for(i = pr->et; i->next; i = i->next);

    i->next = np.et;
}

void eval_tree_wrap(parse_res *pr, const Fun *f, unsigned int argn){
    eval_tree *et = eval_make(f);
    eval_add_arg(et, pr->et->t);
    pr->et->t = et;
    pr->et->t->argn = argn;
}

void skip_el(const char **input){
    while(**input == '\n' || **input == ')' || **input == ' ' || **input == '\t')
        (*input)++;
}

void pattern_add_matches(pattern *p, const dict *local){
    p->match = calloc(p->t->argn, sizeof (Fun));
    unsigned int j = p->t->argn-1;
    for(const dict *i = local->next; i; i = i->next, j--){
        if(i->value->t->f->name){
            if(is_const(i->value->t->f->name))
                p->match[j] = i->value->t->f;
        }
    }
}

parse_res parse_fun(const dict *glob, const char **in, const token_list **left){
    const token_list *input = *left ? *left : tokenize(in);
    parse_res a = parse_tan(&input);
#ifdef LOGALL
    log("&&Parsing function %s of type ",a.et->f->name);
    log_t(a.et->t->f->type);
    log(" &&\n");
#endif
    parse_res c = {NULL, NULL};
    c.type = a.et->t->f->type;
    while(true){
        skip_el(in);
        if(**in == '\0')
            break;
        input = tokenize(in);
        if(*input->next->begin == ':'){
            *left = input;
            break;
        }
        const dict **l = malloc(sizeof (dict*));
        parse_left(a.et->t->f,l,&input);
        unsigned int argn = (*l) ? (**l).value->t->f->lid : 0;
        dict_add(l, a.et->t->f);
        parse_res t = parse_right(a.et->t->f->type, *l, glob, &input);

        eval_tree_wrap(&t, a.et->t->f, argn);
        pattern_add_matches(t.et,*l);
        pattern_add(&c, t);
    }
    return c;
}

const dict* parse_all(const char *input){
    const dict *glob = init();
    const token_list **tl = malloc(sizeof (token_list *));
    while(input && *input != '\0'){

        while(*input == '\n' || *input == ')' || *input == ' ' || *input == '\t')
            input++;
        if(*input == '\0') break;
        parse_res f = parse_fun(glob, &input, tl);
        dict_add_eval(&glob, f.et);
    }
    return glob;
}

