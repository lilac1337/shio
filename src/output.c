#include "output.h"
#include "shio.h"
#include "terminal.h"

#include <stdint.h>

void drawrows(sv *txt) {
    int32_t y;
    
    for (y = 0; y < c.sr; ++y) {
        if (y == c.sr / 3) {
            char welcome[80];
            size_t welcomelen = (size_t)snprintf(welcome, sizeof(welcome), "shio -- version %s", SHIO_VERSION);
            if (welcomelen > c.sc)
                welcomelen = c.sc;
            
            int32_t pad = (c.sc - welcomelen) / 2;
            if (pad) {
                svappend(txt, "~", 1);
                --pad;
            }
            while (pad--) svappend(txt, " ", 1);

            svappend(txt, welcome, welcomelen);
        } else {
            svappend(txt, "~", 1);
        }

        svappend(txt, VT100EL, 3);
        if (y < c.sr - 1)
            svappend(txt, "\r\n", 2);
    }
}

void refreshscreen() {
    sv txt = SV_INIT;

    svappend(&txt, VT100RM25, 6);
    svappend(&txt, VT100CUP, 3);

    drawrows(&txt);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", c.cy + 1, c.cx + 1);
    svappend(&txt, buf, strlen(buf));
    
    svappend(&txt, VT100SM25, 6);

    write(STDOUT_FILENO, txt.s, txt.len);
    svfree(&txt);
}
