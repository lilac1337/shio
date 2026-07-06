#include "row.h"

#include "output.h"
#include "syntax.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

void updatelinenumoffset() {
    c.lno = (u32)snprintf(NULL, 0, "%" PRIu32, c.nrows + 1u);
}

row *getrow(size_t idx) {
    if (idx >= c.nrows)
        return NULL;

    return &c.r[idx];
}

u32 cx2rx(row *r, u32 cx) {
    u32 rx = 0;
    u32 j;

    for (j = 0; j < cx; ++j) {
        if (r->chars[j] == '\t')
            rx += (SHIO_TAB_STOP - 1) - (rx % SHIO_TAB_STOP);

        ++rx;
    }

    return rx;
}

u32 rx2cx(row *r, u32 rx) {
    u32 crx = 0;
    u32 cx;

    for (cx = 0; cx < r->size; ++cx) {
        if (r->chars[cx] == '\t')
            crx += (SHIO_TAB_STOP - 1u) - (crx % SHIO_TAB_STOP);

        ++crx;

        if (crx > rx)
            return cx;
    }

    return cx;
}

void updaterow(row *r) {
    u32 tabs = 0;
    size_t j;
    
    for (j = 0; j < r->size; ++j) {
        if (r->chars[j] == '\t') ++tabs;
    }
    
    free(r->render);

    void *ma = NULL;
    ma = malloc(r->size + tabs * (SHIO_TAB_STOP - 1) + 1); // + 3 if linenum

    if (ma == NULL)
        goto error;
    
    r->render = ma;

    size_t idx = 0;

    for (j = 0; j < r->size; ++j) {
        if (r->chars[j] == '\t') {
            r->render[idx++] = ' ';
            while (idx % SHIO_TAB_STOP != 0) r->render[idx++] = ' ';

            continue;
        }

        r->render[idx++] = r->chars[j];
    }

    r->render[idx] = '\0';
    r->rsize = idx;

    updatesyntax(r);

    return;

error:
    setstatus("malloc returned NULL in updaterow(row *r): %s",
              strerror(errno));
}

void insertrow(u32 at, const char *s, size_t len) {
    if (at > c.nrows)
        return;

    void *ra = NULL;
    ra = realloc(c.r, sizeof(row) * (c.nrows + 1));

    if (ra == NULL)
        goto error;
    
    c.r = ra;
    memmove(&c.r[at + 1], &c.r[at], sizeof(row) * (c.nrows - at));

    size_t j;
    for (j = at + 1; j <= c.nrows; ++j)
        c.r[j].idx++;

    c.r[at].idx = at;
    c.r[at].size = len;
    c.r[at].chars = malloc(len + 1);
    
    memcpy(c.r[at].chars, s, len);
    
    c.r[at].chars[len] = '\0';
    c.r[at].rsize = 0u;
    c.r[at].render = NULL;
    c.r[at].hl = NULL;
    c.r[at].hlmlcmt = false;

    updaterow(&c.r[at]);
    updatelinenumoffset();

    ++c.nrows;
    ++c.dirty;

    return;

error:
    setstatus("failed to insert nl at line %" PRIu32
              ", file may be corrupted, do not save: %s",
              at + 1, strerror(errno));
}

void freerow(row *r) {
    free(r->render);
    free(r->chars);
    free(r->hl);
}

void delrow(u32 at) {
    if (at >= c.nrows)
        return;

    freerow(&c.r[at]);
    memmove(&c.r[at], &c.r[at + 1], sizeof(row)  * (c.nrows - at - 1u));

    size_t j;
    for (j = at; j < c.nrows - 1; ++j)
        c.r[j].idx--;

    updatelinenumoffset();
    
    --c.nrows;
    ++c.dirty;
}

void rowinsertchar(row *r, u32 at, i32 ch) {
    if (at > r->size)
        at = (u32)r->size;

    void *ra = NULL;
    ra = realloc(r->chars, r->size + 2);

    if (ra == NULL)
        goto error;
    
    r->chars = ra;
    memmove(&r->chars[at + 1], &r->chars[at], r->size - at + 1u);
    ++r->size;
    r->chars[at] = (char)ch;
    
    updaterow(r);
    
    ++c.dirty;

    return;
    
error:
    setstatus("failed to insert char %c at line %" PRIu32
              ", file may be corrupted, do not save: %s",
              ch, at + 1, strerror(errno));
}

void rowappendstring(row *r, const char *s, size_t len) {
    void *ra = NULL;
    ra = realloc(r->chars, r->size + len + 1);

    if (ra == NULL)
        goto error;
    
    r->chars = ra;
    memcpy(&r->chars[r->size], s, len);
    r->size += len;
    r->chars[r->size] = '\0';

    updaterow(r);

    ++c.dirty;
    
    return;
    
error:
    setstatus("failed to append string at line %" PRIu32
              ", file may be corrupted, do not save: %s",
              c.cur.y + 1, strerror(errno));
}

void rowdelchar(row *r, u32 at) {
    if (at >= r->size)
        return;

    memmove(&r->chars[at], &r->chars[at + 1], r->size - at);
    --r->size;

    updaterow(r);

    ++c.dirty;
}
