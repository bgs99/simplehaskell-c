#pragma once
#include "parser_common.h"

struct syntax_tree accept_fun_type(const char **input);
void print_type(const struct syntax_tree tree);
