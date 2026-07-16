#include "selection.h"

#include "editor.h"
#include "row.h"
#include "output.h"
#include "types.h"

#include <errno.h>

void resetselection() {
    c.slctn.sidx = 0u;
    c.slctn.eidx = 0u;
    c.slctn.roff = 0;
    c.slctn.rnum = MAGIC_SELECTION; // rn this works because it's a very large
                                 // number, so it'll never be reached, but it will
                                 // have to be checked for eventually

    c.select = false;
}

void handleselection(u32 scx, u32 ecx, u32 r) {
    if (!c.select)
        return;

    u32 srx = cx2rx(&c.r[r], scx);
    u32 erx = cx2rx(&c.r[r], ecx);

    c.slctn.sidx = c.slctn.sidx ? c.slctn.sidx : srx;
    c.slctn.eidx = erx > 0 ? erx - 1u : 0u;
    c.slctn.roff += (i32)c.slctn.rnum - (i32)r;
    c.slctn.rnum = r;
}

// TODO: cutting with c.slctn.sidx = 0 causes a crash from cx2rx in scroll
void cutselection() {
    copyselection();

    const size_t diff = c.slctn.eidx - c.slctn.sidx + 1ul;
    row *r = getrow(c.slctn.rnum);
    u32 i;
    u32 cx = rx2cx(r, (u32)c.slctn.sidx);
    
    for (i = 0; i < diff; ++i) {
        editordelchar(r, cx + 1u, c.slctn.rnum, true);
    }
}

void pasteselection(u32 cx, u32 cy) {
    size_t i;
    for (i = 0ul; i < c.copy.len; ++i) {
        editorinsertchar(cx + (u32)i, cy, c.copy.s[i], true);
    }
}

void copyselection() {
    // this needs to be hardened with the c.slctn vars, prob abs value stuff
    if (!c.select || c.slctn.rnum == MAGIC_SELECTION)
        return;
    
    const size_t sz = c.slctn.eidx - c.slctn.sidx + 1u;

    if (c.copy.len <= sz || c.copy.s == NULL)
        c.copy.s = realloc(c.copy.s, sz + 1ul);

    if (c.copy.s == NULL)
        goto error;
    
    const row *r = getrow(c.slctn.rnum);
    
    if (r->rsize - c.slctn.sidx < sz)
        return;

    const size_t off = sizeof(char) * c.slctn.sidx;

    strncpy(c.copy.s, r->render + off, sz);
    c.copy.s[sz] = '\0';
    c.copy.len = sz;

    setstatus("copied \"%s\"", c.copy.s);
    
    return;

error:
    setstatus("c.copy.s = NULL in copyselection, %s", strerror(errno));
}
