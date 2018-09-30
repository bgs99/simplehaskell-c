#include "hstd.h"
typedef struct parse_res{
  const Fun* val;
  const char* left;
} parse_res;

parse_res parse_tan(const char *input);
parse_res parse_left(Fun *f, dict **local, const char *input);
parse_res parse_right(Fun *f, const dict *local, char * input);

parse_res parse_fun(const char *tan, const char *def);
