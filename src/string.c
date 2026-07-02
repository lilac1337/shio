#include "string.h"

void svappend(sv *s, const char *str, size_t len) {
    char *new = realloc(s->s, s->len + len);

    if (new == NULL)
        return;
    memcpy(&new[s->len], str, len);
    s->s = new;
    s->len += len;
}

void svfree(sv *s) {
    free(s->s);
    s->s = NULL;
    s->len = 0;
}
