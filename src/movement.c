#include "movement.h"
#include "output.h"
#include "row.h"
#include "syntax.h"

void resetselection() {
    c.slctn.sidx = 0u;
    c.slctn.eidx = 0u;
    c.slctn.roff = 0;
    c.slctn.r = 0u;

    c.select = false;
}

void handleselection(u32 scx, u32 ecx, u32 r) {
    if (!c.select)
        return;

    u32 srx = cx2rx(&c.r[r], scx);
    u32 erx = cx2rx(&c.r[r], ecx);

    c.slctn.sidx = c.slctn.sidx ? c.slctn.sidx : srx;
    c.slctn.eidx = erx > 0 ? erx - 1u : 0u;
    c.slctn.roff += (i32)c.slctn.r - (i32)r;
    c.slctn.r = r;
}

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

    handleselection(c.cur.x, ccx, c.cur.y);
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

// TODO: ideal code is wonky after adding tab support, probably need to do something with r->rsize
void movecursor(i32 k) {
    static u32 ideal = 0;
    row *r = getrow(c.cur.y);
    
    switch (k) {
    case ARROW_LEFT:
        if (c.cur.x != 0) {
            --c.cur.x;
            ideal = c.cur.x;
            break;
        }

        if (c.cur.y > 0) {
            --c.cur.y;
            c.cur.x = (u32)c.r[c.cur.y].size;
        }
        
        break;
        
    case ARROW_RIGHT:
        if (r && c.cur.x < r->size) {
            handleselection(c.cur.x, c.cur.x + 1u, c.cur.y);
            ++c.cur.x;
            ideal = c.cur.x;

            break;
        }

        if (r && c.cur.x == r->size && c.cur.y < c.nrows - 1) {
            ++c.cur.y;
            c.cur.x = 0;
        }
        
        break;
        
    case ARROW_UP:
        if (!c.nrows)
            break;

        resetselection();
        
        if (c.cur.y != 0)
            --c.cur.y;

        if (c.r[c.cur.y].size < c.cur.x || c.r[c.cur.y].size < ideal) {
            c.cur.x = (u32)c.r[c.cur.y].size;
            break;
        }

        c.cur.x = MAX(ideal, c.cur.x);
        
        break;
        
    case ARROW_DOWN:
        if (!c.nrows)
            break;

        resetselection();
        
        if (c.cur.y < c.nrows - 1)
            ++c.cur.y;

        if (c.cur.y < c.nrows) {
            if (c.r[c.cur.y].size < c.cur.x || c.r[c.cur.y].size < ideal) {
                c.cur.x = (u32)c.r[c.cur.y].size;
                break;
            }
        
            c.cur.x = MAX(ideal, c.cur.x);
        }
        
        break;
    }
}
