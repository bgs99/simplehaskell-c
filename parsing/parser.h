#include "parser_common.h"

struct syntax_tree accept_block(const char **input);
struct syntax_tree accept_expression(const char **input);
struct syntax_tree accept_file(const char *filename);

