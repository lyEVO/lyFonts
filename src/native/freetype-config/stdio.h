#ifndef LYSIS_STDIO_H
#define LYSIS_STDIO_H

#include <stddef.h>

#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct _lysis_file FILE;

FILE* fopen(const char* path, const char* mode);
int fclose(FILE* f);
size_t fread(void* buf, size_t size, size_t count, FILE* f);
size_t fwrite(const void* buf, size_t size, size_t count, FILE* f);
int fseek(FILE* f, long offset, int origin);
long ftell(FILE* f);
int snprintf(char* buf, size_t size, const char* fmt, ...);

#endif
