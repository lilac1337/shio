#include "input.h"

#include "commands.h"
#include "editor.h"
#include "file.h"
#include "movement.h"
#include "output.h"
#include "row.h"
#include "search.h"
#include "shio.h"
#include "terminal.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

char *prompt(const char *p, callback cb) {
    size_t sz = 128ul;
    void *ma = NULL;

    ma = malloc(sz);
    
    if (ma == NULL)
        goto error;
    
    char *buf = ma;

    size_t len = 0ul;
    buf[0] = '\0';

    while (true) {
        setstatus(p, buf);
        refreshscreen();

        i32 ch = readkey();

        if (ch == DEL_KEY || ch == CTRL_KEY('h') || ch == BACKSPACE) {
            if (len != 0ul)
                buf[--len] = '\0';

            continue;
        }

        if (ch == '\x1b' || ch == CTRL_KEY('g')) {
            setstatus("");

            if (cb)
                cb(buf, (u32)ch);
            
            free(buf);
            
            return NULL;
        }

        if (ch == '\r') {
            if (len != 0ul) {
                setstatus("");

                if (cb)
                    cb(buf, (u32)ch);
                
                return buf;
            }

            continue;
        }

        if (ch < 128 && !iscntrl(ch)) {
            if (len == sz - 1ul) {
                sz *= 2ul;
                buf = realloc(buf, sz);
            }

            buf[len++] = (char)ch;
            buf[len] = '\0';
        }

        if (cb)
            cb(buf, (u32)ch);
    }

    return NULL;
    
error:
    setstatus("malloc returned NULL in prompt: %s", strerror(errno));
    
    return NULL;
}

void processkeypress() {
    // a static var with last keypress for C-x should be good enough for basic
    // emacs-like chords, it was unironically revealed to me in a dream
    static u32 qt = SHIO_QUIT_TIMES;
    static i32 lch = 0;
    
    i32 ch = readkey();

    switch (ch) {
    case '\r':
        editorinsertnl();
        break;
        
    case CTRL_KEY('c'):
        if (lch != CTRL_KEY('x') && qt >= SHIO_QUIT_TIMES) {
            return;
        }
        
        if (c.dirty && qt > 0) {
            setstatus("warning: file has unsaved changes, press C-c %" PRIu32 " times to quit", qt);
            --qt;

            lch = ch;
            return;
        }
        
        v100ed();
        v100cup();
        
        exit(0);
        break;

    case CTRL_KEY('x'):
        setstatus("C-x-");

        break;

    case CTRL_KEY('g'):
        qt = SHIO_QUIT_TIMES;
        setstatus("cancelled");
        break;

    case HOME_KEY:
        c.cur.x = 0;
        break;

    case END_KEY:
        if (c.cur.y < c.nrows)
            c.cur.x = (u32)c.r[c.cur.y].size;
        
        break;

    case CTRL_KEY('s'):
        if (lch == CTRL_KEY('x')) {
            save();
            
            break;
        }
        
        search();
        break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
        if (ch == DEL_KEY)
            movecursor(ARROW_RIGHT);

        editordelchar(&c.r[c.cur.y], c.cur.x, c.cur.y, true);
        
        break;

    case PAGE_UP: {
        c.cur.y = c.roff;
        
        break;
    }
        
    case PAGE_DOWN: {
        c.cur.y = c.roff + c.ws.r - 1u;

        if (c.cur.y > c.nrows)
            c.cur.y = c.nrows - 1u;
        
        break;
    }

    case META_X: {
        //setstatus("M-x");
        executecommand();

        break;
    }

    case META_F: {
        wordfoward();
        break;
    }

    case META_B: {
        wordbackward();
        break;
    }

    case CTRL_KEY(' '): {
        setstatus("%s", c.select ? "mark deactivated" : "mark set");

        if (c.select) {
            resetselection();
            break;
        }
            
        c.select = true;
        
        break;
    }

    case META_W: {
        copyselection();

        break;
    }

    case CTRL_KEY('y'): {
        pasteselection(c.cur.x, c.cur.y);

        break;
    }

    case CTRL_KEY('w'): {
        cutselection();

        break;
    }
                
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        movecursor(ch);
        break;

    case CTRL_KEY('l'):
    case '\x1b':
        break;

    default:
        editorinsertchar(c.cur.x, c.cur.y, ch, true);
        break;
    }

    qt = SHIO_QUIT_TIMES;
    lch = ch;
}
