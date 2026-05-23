#include "input.h"

#include <stdlib.h>

#include "terminal.h"

void movecursor(i32 k) {
    switch (k) {
    case ARROW_LEFT:
        if (c.cur.x != 0)
            --c.cur.x;
        break;
    case ARROW_RIGHT:
        if (c.cur.x != c.ws.c - 1)
            ++c.cur.x;
        break;
    case ARROW_UP:
        if (c.cur.y != 0)
            --c.cur.y;
        break;
    case ARROW_DOWN:
        if (c.cur.y != c.ws.r - 1)
            ++c.cur.y;
        break;
    }
}

void processkeypress() {
    i32 ch = readkey();

    switch (ch) {
    case CTRL_KEY('q'):
        v100ed();
        v100cup();
        
        exit(0);
        break;

    case HOME_KEY:
        c.cur.x = 0;
        break;

    case END_KEY:
        c.cur.x = c.ws.c - 1;
        break;

    case PAGE_UP: {
        i32 t = c.cur.y + 1;
        while (--t)
            movecursor(ARROW_UP);
        break;
    }
    case PAGE_DOWN: {
        i32 t = c.ws.r - c.cur.y;
        while (--t)
            movecursor(ARROW_DOWN);
        break;
    }
        
        
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        movecursor(ch);
        break;
    }
}
