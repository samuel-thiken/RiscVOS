#pragma once

#include <stddef.h>

__attribute__((noreturn)) void abort(void);
__attribute__((noreturn)) void exit(int status);

void* malloc(size_t size);
void* calloc(size_t n_elements, size_t element_size);
void* memalign(size_t alignment, size_t size);
void free(void* ptr);
long int strtol(const char *nptr, char **endptr, int base);
