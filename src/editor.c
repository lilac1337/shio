#include "editor.h"

#include "row.h"

void editorinsertchar(u32 cx, u32 cy, i32 ch, bool at) {
    if (c.cur.y == c.nrows)
        insertrow(c.nrows, "", 0);

    rowinsertchar(&c.r[cy], cx, ch);
    c.cur.x = (at) ? c.cur.x + 1 : c.cur.x;
}

void editorinsertnl() {
    if (c.cur.x == 0) {
        insertrow(c.cur.y, "", 0);
        
        goto end;
    }

    row *r = &c.r[c.cur.y];
    
    insertrow(c.cur.y + 1, &r->chars[c.cur.x], r->size - c.cur.x);
    
    r = &c.r[c.cur.y];
    r->size = c.cur.x;
    r->chars[r->size] = '\0';

    updaterow(r);

end:
    ++c.cur.y;
    c.cur.x = 0;
}

void editordelchar(row *r, u32 cx, u32 cy, bool at) {
    if (cy == c.nrows || (cy == 0 && cx == 0))
        return;

    //row *r = &c.r[c.cur.y];

    if (cx == 0) {
        c.cur.x = (at) ? (u32)c.r[cy - 1u].size : c.cur.x;

        rowappendstring(&c.r[cy - 1u], r->chars, r->size);
        delrow(cy);

        c.cur.y = (at) ? c.cur.y - 1u : c.cur.y;

        return;
    }
    
    rowdelchar(r, cx - 1u);
    c.cur.x = (at) ? c.cur.x - 1u : c.cur.x;
}

void editorkillrow(u32 cx, u32 cy) {
    row *r = getrow(cy);
    u32 i;
    size_t diff = r->size - cx;
    
    for (i = 0; i < diff; ++i) {
        editordelchar(r, cx + 1u, cy, false);
    }
}
