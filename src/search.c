#include "search.h"

#include "input.h"
#include "row.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>

// TODO: make this code nicer, i hate else if and i want lm & dir to be u32
// also, make it so it doesn't scroll all the way down to the result, maybe 2 lines above
void searchcallback(char *q, u32 k) {
    static i32 lm = -1;
    static i32 dir = 1;

    static ssize_t shlline;
    static u8 *shl = NULL;

    if (shl) {
        memcpy(c.r[shlline].hl, shl, c.r[shlline].rsize);
        free(shl);
        shl = NULL;
    }
    
    if (k == '\r' || k == '\x1b' || k == CTRL_KEY('g')) {
        lm = -1;
        dir = 1;
        
        return;
    } else if (k == ARROW_RIGHT || k == ARROW_DOWN)
        dir = 1;

    else if (k == ARROW_LEFT || k == ARROW_UP)
        dir = -1;

    else if (lm == -1)
        dir = 1;

    else {
        lm = -1;
        dir = 1;
    }
    
    i32 cur = lm;
    u32 i;

    for (i = 0u; i < c.nrows; ++i) {
        cur += dir;

        if (cur == -1)
            cur = (i32)c.nrows - 1;

        if (cur >= (i32)c.nrows)
            cur = 0;
        
        row *r = &c.r[cur];
        char *m = strstr(r->render, q);

        if (!m)
            continue;

        lm = cur;
        c.cur.y = (u32)cur;
        c.cur.x = rx2cx(r, (u32)(m - r->render));
        c.roff = c.nrows;

        shlline = cur;
        shl = malloc(r->rsize);
        memcpy(shl, r->hl, r->rsize);
        memset(&r->hl[m - r->render], HL_MATCH, strlen(q));

        break;
    }
}

void search() {
    u32 scx = c.cur.x;
    u32 scy = c.cur.y;
    u32 scoff = c.coff;
    u32 sroff = c.roff;
    
    char *q = prompt("search: %s (C-g to cancel, arrow keys to change)", searchcallback);

    if (q) {
        free(q);

        return;
    }

    c.cur.x = scx;
    c.cur.y = scy;
    c.coff = scoff;
    c.roff = sroff;
}
