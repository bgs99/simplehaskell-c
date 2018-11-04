#pragma once
#include "list.h"
#include "dictionary_t.h"

void free_all(dict *d);
void generics_free(generics *g);
void mark_ptr(void *ptr);
