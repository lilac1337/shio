#include "output.h"

#include "string.h"
#include "terminal.h"

inline void endrow(u32 cur, sv *txt) {
    svappend(txt, VT100EL, 3);
    if (cur < c.ws.r - 1)
        svappend(txt, "\r\n", 2); 
}

void scroll() {
    if (c.cur.y < c.roff)
        c.roff = c.cur.y;

    if (c.cur.y >= c.roff + c.ws.r)
        c.roff = c.cur.y - c.ws.r + 1;
}

void drawrows(sv *txt) {
    u32 y;
    
    for (y = 0; y < c.ws.r; ++y) {
        u32 frow = y + c.roff;

        if (frow >= c.nrows) {
            if (c.nrows == 0 && y == c.ws.r / 3) {
                char welcome[80];
                size_t welcomelen = (size_t)snprintf(welcome, sizeof(welcome), "shio -- version %s", SHIO_VERSION);
                if (welcomelen > c.ws.c)
                    welcomelen = c.ws.c;
            
                i32 pad = (c.ws.c - welcomelen) / 2;
                if (pad) {
                    svappend(txt, "~", 1);
                    --pad;
                }
                while (pad--) svappend(txt, " ", 1);

                svappend(txt, welcome, welcomelen);
            } else {
                svappend(txt, "~", 1);
            }

            endrow(y, txt);
            continue;
        }
        
        size_t len = c.r[frow].size;
            
        if (len > c.ws.c)
            len = c.ws.c;

        svappend(txt, c.r[frow].chars, len);
        endrow(y, txt);
    }
}

void refreshscreen() {
    scroll();
    
    sv txt = SV_INIT;

    svappend(&txt, VT100RM25, 6);
    svappend(&txt, VT100CUP, 3);

    drawrows(&txt);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%" PRId32 ";%" PRId32 "H", c.cur.y + 1, c.cur.x + 1);
    svappend(&txt, buf, strlen(buf));
    
    svappend(&txt, VT100SM25, 6);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-result"
    write(STDOUT_FILENO, txt.s, txt.len);

    #pragma GCC diagnostic pop
    
    svfree(&txt);
}
