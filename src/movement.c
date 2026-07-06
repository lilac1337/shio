#include "movement.h"
#include "output.h"
#include "row.h"
#include "syntax.h"

// TODO: allow M-f and M-b to go into other rows
void wordfoward() {
    row *r = getrow(c.cur.y);
    u32 ccx = c.cur.x;
    
    if (!r)
        goto error;

loop:
    while (!isseparator(r->chars[ccx])) {
        ++ccx;
    }

    if (ccx == c.cur.x && r->size >= ccx + 1u) {
        ++ccx;
        goto loop;
    }
    
    c.cur.x = ccx;
    
    return;
    
error:
    setstatus("M-f couldn't find row.");
}

void wordbackward() {
    row *r = getrow(c.cur.y);
    u32 ccx = c.cur.x;
    
    if (!r)
        goto error;

loop:
    while (ccx != 0u && !isseparator(r->chars[ccx])) {
        --ccx;
    }

    if (ccx == c.cur.x && 0u < ccx) {
        --ccx;
        goto loop;
    }
    
    c.cur.x = ccx;
    
    return;
    
error:
    setstatus("M-b couldn't find row.");
}
