#include "row.h"

#include <malloc.h>
#include <memory.h>

void appendrow(char *s, size_t len) {
    const i32 idx = c.nrows;
    
    c.r = realloc(c.r, sizeof(row) * (c.nrows + 1));
    
    c.r[idx].size = len;
    c.r[idx].chars = malloc(len + 1);
    
    memcpy(c.r[idx].chars, s, len);
    
    c.r[idx].chars[len] = '\0';

    ++c.nrows;
}
