#ifndef SHIO_STRING_H
#define SHIO_STRING_H

#include <malloc.h>
#include <memory.h>

#define SV_INIT {NULL, 0}

typedef struct {
    char *s;
    size_t len;
} sv;

void svappend(sv *sv, const char *s, size_t len);
void svfree(sv *sv);

#endif
