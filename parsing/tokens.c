#include "tokens.h"
#include "malloc.h"
#include "reader.h"

char* get_name(token_list **tl){
    char *name = calloc((*tl)->val->length+1, sizeof (char));
    strncpy(name, (*tl)->val->begin, (*tl)->val->length);
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
 * @brief Splits input line into tokens
 * @param input string
 * @return tokenized input
 */

token_list* tokenize_r(const char **input){
    token_list *ret = NULL;
    while(**input != '\0' && **input != '\n'){
        skip_ws(input);
        token_list *new;

        switch (**input) {
        case '\0':
        case '\n':
            return ret;
        case '@':
            new = malloc(sizeof (token_list));
            new->val = malloc(sizeof (token));
            new->val->type = DATATYPE;
            new->next = ret;
            break;
        case '!':
            new = malloc(sizeof (token_list));
            new->val = malloc(sizeof (token));
            new->val->type = IMPORT;
            new->next = ret;
            break;
        case '(':
            new = malloc(sizeof (token_list));
            new->val = malloc(sizeof (token));
            new->val->type = POPEN;
            new->next = ret;
            break;
        case ')':
            new = malloc(sizeof (token_list));
            new->val = malloc(sizeof (token));
            new->val->type = PCLOSE;
            new->next = ret;
            ret = new;
            break;
        case '-':
        case '|':
            if(ret->val->type == UNDEFINED)
                ret->val->type = TYPE_NAME;

            new = malloc(sizeof(token_list));
            new->val = malloc(sizeof (token));
            new->val->type = DELIMITER;
            new->next = ret;
            ret = new;
            break;
        case '=':
        case ':':
            if(ret->val->type == UNDEFINED)
                ret->val->type = FUN_NAME;

            new = malloc(sizeof(token_list));
            new->val = malloc(sizeof (token));
            new->val->type = DELIMITER;
            new->next = ret;
            ret = new;
            break;
        default:
            new = malloc(sizeof (token_list));
            new->val = malloc(sizeof (token));
            new->val->begin = *input;
            read_word(NULL,input);
            new->val->length = (unsigned)(*input - new->val->begin);
            new->next = ret;
            ret = new;
            (*input)--;
        }
        (*input)++;
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
