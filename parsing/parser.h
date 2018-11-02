#include "parser_common.h"

struct syntax_tree accept_program(const char **input);
struct syntax_tree accept_expression(const char **input);
struct syntax_tree accept_file(const char *filename);

char *get_name(struct syntax_tree tree);
