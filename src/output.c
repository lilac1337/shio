#include "output.h"

#include "row.h"
#include "string.h"
#include "syntax.h"
#include "terminal.h"
#include "types.h"

#include <ctype.h>
#include <stdarg.h>

static inline void endrow(sv *txt) {
    svappend(txt, VT100EL, 3);
    svappend(txt, "\r\n", 2); 
}

void scroll() {
    c.rx = 0;

    if (c.cur.y < c.nrows)
        c.rx = cx2rx(&c.r[c.cur.y], c.cur.x);
    
    if (c.cur.y < c.roff)
        c.roff = c.cur.y;

    if (c.cur.y >= c.roff + c.ws.r)
        c.roff = c.cur.y - c.ws.r + 1;

    if (c.rx < c.coff)
        c.coff = c.rx;

    if (c.rx >= c.coff + c.ws.c)
        c.coff = c.rx - c.ws.c + 1;
}

void drawlinenum(sv *txt, u32 ln) {
    char linenum[32];
    u32 linenumcol = (ln == c.cur.y + 1u) ? syntax2color(HL_MATCH) : syntax2color(HL_LINENUM);
    int numlen = snprintf(linenum, sizeof(linenum), "\x1b[%" PRIu32 "m%*" PRIu32 " ", linenumcol, c.lno, ln);
    svappend(txt, linenum, (size_t)numlen);
    svappend(txt, VT100COLORDEFAULT, 5);
}

void drawsyntaxhl(sv *txt, u32 frow, size_t len) {
    char *ch = &c.r[frow].render[c.coff];
    u8 *hl = &c.r[frow].hl[c.coff];
    u32 curcol = HL_NORMAL;
    size_t j;

    //row *r = getrow(frow);
    //u32 slcscx = cx2rx(&c.r[frow], (u32)c.slctn.sidx);
    //u32 slcecx = cx2rx(&c.r[frow], (u32)c.slctn.eidx);
        
    for (j = 0; j < len; ++j) {
        if (iscntrl(ch[j])) {
            char sym = (ch[j] <= 26) ? '@' + ch[j] : '?';
                
            svappend(txt, VT100SGR7, 4ul);
            svappend(txt, &sym, 1ul);
            svappend(txt, VT100SGR, 3ul);

            if (curcol != HL_NORMAL) {
                char buf[16];
                int clen = snprintf(buf, sizeof(buf), "\x1b%" PRIu32 "m", curcol);
                svappend(txt, buf, (size_t)clen);
            }

            continue;
        }

        if (frow == c.slctn.r && c.select && c.slctn.sidx <= j && c.slctn.eidx >= j) {
            svappend(txt, VT100SGR7, 4ul);
            svappend(txt, &ch[j], 1ul);
            svappend(txt, VT100SGR, 3ul);

            if (curcol != HL_NORMAL) {
                char buf[16];
                int clen = snprintf(buf, sizeof(buf), "\x1b%" PRIu32 "m", curcol);
                svappend(txt, buf, (size_t)clen);
            }

            continue;
        }
            
        if (hl[j] == HL_NORMAL) {
            if (curcol != HL_NORMAL) {
                svappend(txt, VT100COLORDEFAULT, 5ul);
                curcol = HL_NORMAL;
            }
                
            svappend(txt, &ch[j], 1ul);

            continue;
        }
            
        u32 color = syntax2color(hl[j]);
        if (curcol != color) {
            char buf[16];
            int clen = snprintf(buf, sizeof(buf), "\x1b[%" PRIu32 "m", color);
            
            svappend(txt, buf, (size_t)clen);
            curcol = color;
        }
            
        svappend(txt, &ch[j], 1ul);
    }
}

void drawwelcometxt(sv *txt) {
    char welcome[80];
    size_t welcomelen = (size_t)snprintf(welcome, sizeof(welcome), "shio -- version %s", SHIO_VERSION);
    if (welcomelen > c.ws.c)
        welcomelen = c.ws.c;
            
    size_t pad = (c.ws.c - welcomelen) / 2;
    if (pad) {
        svappend(txt, "~", 1);
        --pad;
    }
    while (pad--) svappend(txt, " ", 1);

    svappend(txt, welcome, welcomelen);
}

// TODO: separate this into multiple smaller functions
void drawrows(sv *txt) {
    u32 y;
    
    for (y = 0; y < c.ws.r; ++y) {
        u32 frow = y + c.roff;

        if (frow >= c.nrows) {
            if (c.nrows == 0 && y == c.ws.r / 3) {
                drawwelcometxt(txt);
            } else {
                svappend(txt, "~", 1);
            }

            endrow(txt);
            continue;
        }
        
        size_t len = 0;
        
        if ((size_t)c.coff < c.r[frow].rsize)
            len = c.r[frow].rsize - (size_t)c.coff;
        if (len > c.ws.c)
            len = c.ws.c;

        // experimental line number code
        drawlinenum(txt, y + c.roff + 1u);
        drawsyntaxhl(txt, frow, len);
        
        svappend(txt, VT100COLORDEFAULT, 5ul);
        endrow(txt);
    }
}

void drawstatus(sv *txt) {
    // https://vt100.net/docs/vt100-ug/chapter3.html#SGR
    svappend(txt, VT100SGR7, 4);

    char status[80], rstatus[80];
    ssize_t len = snprintf(status, sizeof(status), "%.20s - %" PRIu32 " lines %s",
                           c.fn ? c.fn : "*scratch*", c.nrows,
                           c.dirty ? "**" : "--");

    ssize_t rlen = snprintf(rstatus, sizeof(rstatus), "~~shio~~ | .%s | ch%" PRIu32 ", li%" PRIu32 "/%" PRIu32,
                            c.syn ? c.syn->ft : "no ft", c.cur.x, c.cur.y + 1, c.nrows);

    if (len < 0 || rlen < 0)
        die("snprintf in drawstatus");
    
    if (len > c.ws.c)
        len = c.ws.c;

    svappend(txt, status, (size_t)len);
    
    while (len < c.ws.c) {
        if (c.ws.c - len == rlen) {
            svappend(txt, rstatus, (size_t)rlen);
            break;
        }
        
        svappend(txt, " ", 1);
        ++len;
    }

    svappend(txt, VT100SGR, 3);
    svappend(txt, "\r\n", 2);
}

void drawmessage(sv *txt) {
    svappend(txt, VT100EL, 3);

    size_t len = strlen(c.status);

    if (len > c.ws.c)
        len = c.ws.c;

    if (len && time(NULL) - c.time < 5)
        svappend(txt, c.status, len);
}

void drawcursor(sv *txt) {
    const u32 cy = c.cur.y - c.roff + 1u;
    const u32 cx = c.rx + c.lno - c.coff + 2u; // i'm not exactly sure why we have to do +2, but it works
    
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%" PRIu32 ";%" PRIu32 "H", cy, cx);
    
    svappend(txt, buf, strlen(buf));
}

void refreshscreen() {
    scroll();
    
    sv txt = SV_INIT;

    svappend(&txt, VT100RM25, 6);
    svappend(&txt, VT100CUP, 3);

    drawrows(&txt);
    drawstatus(&txt);
    drawmessage(&txt);
    drawcursor(&txt);
    
    svappend(&txt, VT100SM25, 6);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-result"
    write(STDOUT_FILENO, txt.s, txt.len);

    #pragma GCC diagnostic pop
    
    svfree(&txt);
}

void setstatus(const char *fmt, ...) {
    va_list ap;
    
    va_start(ap, fmt);
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-nonliteral"
    vsnprintf(c.status, sizeof(c.status), fmt, ap);
    #pragma GCC diagnostic pop
    va_end(ap);

    c.time = time(NULL);
}
