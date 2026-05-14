#include "input.h"

#include "shio.h"
#include "terminal.h"

void movecursor(int32_t k) {
    switch (k) {
    case ARROW_LEFT:
        if (c.cx != 0)
            --c.cx;
        break;
    case ARROW_RIGHT:
        if (c.cx != c.sc - 1)
            ++c.cx;
        break;
    case ARROW_UP:
        if (c.cy != 0)
            --c.cy;
        break;
    case ARROW_DOWN:
        if (c.cy != c.sr - 1)
            ++c.cy;
        break;
    }
}

void processkeypress() {
    int32_t ch = readkey();

    switch (ch) {
    case CTRL_KEY('q'):
        v100ed();
        v100cup();
        
        exit(0);
        break;

    case PAGE_UP: {
        int32_t t = c.cy + 1;
        while (--t)
            movecursor(ARROW_UP);
        break;
    }
    case PAGE_DOWN: {
        int32_t t = c.sr - c.cy;
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
