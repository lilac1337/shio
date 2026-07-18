#include "movement.h"
#include "editor.h"
#include "output.h"
#include "row.h"
#include "selection.h"
#include "shio.h"
#include "syntax.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

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

    handleselection(c.cur.x, ccx, c.cur.y);
    c.cur.x = ccx;
    
    return;
    
error:
    setstatus("M-b couldn't find row.");
}

// TODO: ideal code is wonky after adding tab support, probably need to do something with r->rsize
void movecursor(i32 k) {
//    static u32 ideal = 0u;
//    static u32 ltabs = 0u;
    row *r = getrow(c.cur.y);

    switch (k) {
    case ARROW_LEFT:
        if (c.cur.x != 0u) {
            handleselection(c.cur.x, c.cur.x - 1u, c.cur.y);
            --c.cur.x;
//            ideal = c.cur.x;
            break;
        }

        if (c.cur.y > 0u) {
            --c.cur.y;
            c.cur.x = (u32)c.r[c.cur.y].size;
        }
        
        break;
        
    case ARROW_RIGHT:
        if (r && c.cur.x < r->size) {
            handleselection(c.cur.x, c.cur.x + 1u, c.cur.y);
            ++c.cur.x;
            //ideal = c.cur.x;

            break;
        }

        if (r && c.cur.x == r->size && c.cur.y < c.nrows - 1u) {
            ++c.cur.y;
            c.cur.x = 0u;
        }
        
        break;
        
    case ARROW_UP:
        if (!c.nrows)
            break;

        resetselection();
        
        if (c.cur.y != 0u)
            --c.cur.y;

        if (c.r[c.cur.y].size < c.cur.x) {// || c.r[c.cur.y].size < ideal) {
            c.cur.x = (u32)c.r[c.cur.y].size;
            break;
        }

        //c.cur.x = MAX(idealcalc(r, c.cur.x, ideal, ltabs), c.cur.x);
        
        break;
        
    case ARROW_DOWN:
        if (!c.nrows)
            break;

        resetselection();
        
        if (c.cur.y < c.nrows - 1)
            ++c.cur.y;

        if (c.cur.y < c.nrows) {
            if (c.r[c.cur.y].size < c.cur.x) {// || c.r[c.cur.y].size < ideal) {
                c.cur.x = (u32)c.r[c.cur.y].size;
                break;
            }
        
            //c.cur.x = MAX(idealcalc(r, c.cur.x, ideal, ltabs), c.cur.x);
        }
        
        break;
    }
}
