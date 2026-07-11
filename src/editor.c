#include "editor.h"

#include "row.h"

void editorinsertchar(row *r, u32 cx, u32 cy, i32 ch, bool at) {
    if (cy == c.nrows)
        insertrow(c.nrows, "", 0);

    rowinsertchar(r, cx, ch);
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

void editordelchar() {
    if (c.cur.y == c.nrows || (c.cur.y == 0 && c.cur.x == 0))
        return;

    row *r = &c.r[c.cur.y];

    if (c.cur.x == 0) {
        c.cur.x = (u32)c.r[c.cur.y - 1].size;

        rowappendstring(&c.r[c.cur.y - 1], r->chars, r->size);
        delrow(c.cur.y);

        --c.cur.y;

        return;
    }
    
    rowdelchar(r, c.cur.x - 1);
    --c.cur.x;
}
