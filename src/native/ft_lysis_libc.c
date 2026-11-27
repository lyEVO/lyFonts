/*
 *  This file is part of lysis fonts.
 *  Copyright © 2026 lysis evolutions
 *
 *  https://github.com/lyEVO/lyFonts
 */
#include <stddef.h>
#include <stdarg.h>

#include "freetype-config/string.h"
#include "freetype-config/stdlib.h"
#include "freetype-config/stdio.h"

/* Cosmos heap */
extern void* __cosmos_heap_alloc(size_t size);
extern void __cosmos_heap_free(void* ptr);

void* memchr(const void* s, int c, size_t n)
{
    const unsigned char* p = (const unsigned char*)s;
    size_t i;
    for (i = 0; i < n; i++)
    {
        if (p[i] == (unsigned char)c)
            return (void*)(p + i);
    }
    return NULL;
}

size_t strlen(const char* s)
{
    size_t n = 0;
    while (s[n] != '\0')
        n++;
    return n;
}

char* strcpy(char* dest, const char* src)
{
    size_t i = 0;
    while (1)
    {
        dest[i] = src[i];
        if (src[i] == '\0')
            break;
        i++;
    }
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}

char* strcat(char* dest, const char* src)
{
    size_t dlen = strlen(dest);
    size_t i = 0;
    while (1)
    {
        dest[dlen + i] = src[i];
        if (src[i] == '\0')
            break;
        i++;
    }
    return dest;
}

int strcmp(const char* a, const char* b)
{
    while (*a != '\0' && *a == *b)
    {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char* a, const char* b, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
    {
        if (a[i] != b[i] || a[i] == '\0')
            return (unsigned char)a[i] - (unsigned char)b[i];
    }
    return 0;
}

char* strrchr(const char* s, int c)
{
    const char* last = NULL;
    while (1)
    {
        if (*s == (char)c)
            last = s;
        if (*s == '\0')
            break;
        s++;
    }
    return (char*)last;
}

char* strstr(const char* haystack, const char* needle)
{
    size_t nlen = strlen(needle);
    size_t i;
    if (nlen == 0)
        return (char*)haystack;
    for (i = 0; haystack[i] != '\0'; i++)
    {
        if (strncmp(haystack + i, needle, nlen) == 0)
            return (char*)(haystack + i);
    }
    return NULL;
}

void* malloc(size_t size)
{
    size_t total = size + sizeof(size_t);
    size_t* block = (size_t*)__cosmos_heap_alloc(total);
    if (block == NULL)
        return NULL;
    block[0] = size;
    return (void*)(block + 1);
}

void free(void* ptr)
{
    if (ptr == NULL)
        return;
    __cosmos_heap_free((size_t*)ptr - 1);
}

void* realloc(void* ptr, size_t size)
{
    size_t oldSize;
    void* newPtr;
    size_t copySize;
    if (ptr == NULL)
        return malloc(size);
    if (size == 0)
    {
        free(ptr);
        return NULL;
    }
    oldSize = ((size_t*)ptr)[-1];
    newPtr = malloc(size);
    if (newPtr == NULL)
        return NULL;
    copySize = oldSize < size ? oldSize : size;
    memcpy(newPtr, ptr, copySize);
    free(ptr);
    return newPtr;
}

void* calloc(size_t count, size_t size)
{
    size_t total = count * size;
    void* ptr = malloc(total);
    if (ptr != NULL)
        memset(ptr, 0, total);
    return ptr;
}

void qsort(void* base, size_t count, size_t size, int (*compare)(const void*, const void*))
{
    unsigned char* arr = (unsigned char*)base;
    unsigned char* tmp;
    size_t i;
    size_t j;
    if (count < 2 || size == 0)
        return;
    tmp = (unsigned char*)malloc(size);
    if (tmp == NULL)
        return;
    for (i = 1; i < count; i++)
    {
        memcpy(tmp, arr + i * size, size);
        j = i;
        while (j > 0 && compare(arr + (j - 1) * size, tmp) > 0)
        {
            memcpy(arr + j * size, arr + (j - 1) * size, size);
            j--;
        }
        memcpy(arr + j * size, tmp, size);
    }
    free(tmp);
}

long strtol(const char* s, char** end, int base)
{
    long value = 0;
    int negative = 0;
    const char* p = s;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    if (*p == '-')
    {
        negative = 1;
        p++;
    }
    else if (*p == '+')
    {
        p++;
    }
    if ((base == 0 || base == 16) && p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
    {
        base = 16;
        p += 2;
    }
    else if (base == 0)
    {
        base = 10;
    }
    while (1)
    {
        int digit;
        if (*p >= '0' && *p <= '9')
            digit = *p - '0';
        else if (*p >= 'a' && *p <= 'f')
            digit = *p - 'a' + 10;
        else if (*p >= 'A' && *p <= 'F')
            digit = *p - 'A' + 10;
        else
            break;
        if (digit >= base)
            break;
        value = value * base + digit;
        p++;
    }
    if (end != NULL)
        *end = (char*)p;
    return negative ? -value : value;
}

char* getenv(const char* name)
{
    (void)name;
    return NULL;
}

void abort(void)
{
    for (;;)
    {
    }
}

FILE* fopen(const char* path, const char* mode)
{
    (void)path;
    (void)mode;
    return NULL;
}

int fclose(FILE* f)
{
    (void)f;
    return 0;
}

size_t fread(void* buf, size_t size, size_t count, FILE* f)
{
    (void)buf;
    (void)size;
    (void)count;
    (void)f;
    return 0;
}

size_t fwrite(const void* buf, size_t size, size_t count, FILE* f)
{
    (void)buf;
    (void)size;
    (void)count;
    (void)f;
    return 0;
}

int fseek(FILE* f, long offset, int origin)
{
    (void)f;
    (void)offset;
    (void)origin;
    return -1;
}

long ftell(FILE* f)
{
    (void)f;
    return -1;
}

static void lysis_putc(char* buf, size_t size, size_t* pos, char c)
{
    if (*pos + 1 < size)
        buf[*pos] = c;
    (*pos)++;
}

static void lysis_puts(char* buf, size_t size, size_t* pos, const char* s)
{
    while (*s != '\0')
    {
        lysis_putc(buf, size, pos, *s);
        s++;
    }
}

static void lysis_put_uint(char* buf, size_t size, size_t* pos, unsigned long value, int base, int uppercase)
{
    char digits[32];
    int i = 0;
    if (value == 0)
    {
        lysis_putc(buf, size, pos, '0');
        return;
    }
    while (value > 0)
    {
        int d = (int)(value % (unsigned long)base);
        digits[i++] = (char)(d < 10 ? '0' + d : (uppercase ? 'A' : 'a') + d - 10);
        value /= (unsigned long)base;
    }
    while (i > 0)
    {
        i--;
        lysis_putc(buf, size, pos, digits[i]);
    }
}

int snprintf(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    size_t pos = 0;
    size_t i = 0;
    va_start(args, fmt);
    while (fmt[i] != '\0')
    {
        if (fmt[i] != '%')
        {
            lysis_putc(buf, size, &pos, fmt[i]);
            i++;
            continue;
        }
        i++;
        if (fmt[i] == '%')
        {
            lysis_putc(buf, size, &pos, '%');
            i++;
            continue;
        }
        int isLong = 0;
        if (fmt[i] == 'l')
        {
            isLong = 1;
            i++;
        }
        switch (fmt[i])
        {
            case 's':
            {
                const char* s = va_arg(args, const char*);
                lysis_puts(buf, size, &pos, s != NULL ? s : "(null)");
                break;
            }
            case 'd':
            case 'i':
            {
                long value = isLong ? va_arg(args, long) : va_arg(args, int);
                if (value < 0)
                {
                    lysis_putc(buf, size, &pos, '-');
                    lysis_put_uint(buf, size, &pos, (unsigned long)(-value), 10, 0);
                }
                else
                {
                    lysis_put_uint(buf, size, &pos, (unsigned long)value, 10, 0);
                }
                break;
            }
            case 'u':
            {
                unsigned long value = isLong ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                lysis_put_uint(buf, size, &pos, value, 10, 0);
                break;
            }
            case 'x':
            {
                unsigned long value = isLong ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                lysis_put_uint(buf, size, &pos, value, 16, 0);
                break;
            }
            case 'X':
            {
                unsigned long value = isLong ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                lysis_put_uint(buf, size, &pos, value, 16, 1);
                break;
            }
            case 'c':
            {
                int c = va_arg(args, int);
                lysis_putc(buf, size, &pos, (char)c);
                break;
            }
            case 'p':
            {
                void* p = va_arg(args, void*);
                lysis_puts(buf, size, &pos, "0x");
                lysis_put_uint(buf, size, &pos, (unsigned long)p, 16, 0);
                break;
            }
            default:
                break;
        }
        i++;
    }
    va_end(args);
    if (size > 0)
        buf[pos < size ? pos : size - 1] = '\0';
    return (int)pos;
}
