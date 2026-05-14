#include "string.h"

void svappend(sv *sv, const char *s, size_t len) {
    char *new = realloc(sv->s, sv->len + len);

    if (new == NULL)
        return;
    memcpy(&new[sv->len], s, len);
    sv->s = new;
    sv->len += len;
}

void svfree(sv *sv) {
    free(sv->s);
    sv->s = NULL;
    sv->len = 0;
}
