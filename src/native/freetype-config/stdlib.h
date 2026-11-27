#ifndef LYSIS_STDLIB_H
#define LYSIS_STDLIB_H

#include <stddef.h>

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void* calloc(size_t count, size_t size);
void qsort(void* base, size_t count, size_t size, int (*compare)(const void*, const void*));
long strtol(const char* s, char** end, int base);
char* getenv(const char* name);
void abort(void);

#endif
