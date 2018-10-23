#pragma once
#include <stdbool.h>

void skip_ws(const char  **input);
void read_word(char *buff, const char **input);

bool is_const(const char *input);
void skip_el(const char **input);
