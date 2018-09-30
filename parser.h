#include "hstd.h"
typedef struct parse_res{
  Fun* val;
  const char* left;
} parse_res;

parse_res parse_tan(const char *input);
parse_res parse_left(Fun *f, dict **local, const char *input);
parse_res parse_right(Fun *f, const dict *local, const char * input);

parse_res parse_fun(const char *input);

void parse_all(Fun **buff, const char *input);
