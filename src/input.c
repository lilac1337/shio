#include "input.h"

#include "editor.h"
#include "file.h"
#include "output.h"
#include "row.h"
#include "search.h"
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

void processkeypress() {
    static u32 qt = SHIO_QUIT_TIMES;
    
    i32 ch = readkey();

    switch (ch) {
    case '\r':
        editorinsertnl();
        break;
        
    case CTRL_KEY('q'):
        if (c.dirty && qt > 0) {
            setstatus("warning: file has unsaved changes, press C-q %" PRIu32 " times to quit", qt);
            --qt;

            return;
        }
        
        v100ed();
        v100cup();
        
        exit(0);
        break;

    case CTRL_KEY('x'):
        save();
        break;

    case HOME_KEY:
        c.cur.x = 0;
        break;

    case END_KEY:
        if (c.cur.y < c.nrows)
            c.cur.x = (u32)c.r[c.cur.y].size;
        
        break;

    case CTRL_KEY('s'):
        search();
        break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
        if (ch == DEL_KEY)
            movecursor(ARROW_RIGHT);

        editordelchar();
        
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
        editorinsertchar(ch);
        break;
    }

    qt = SHIO_QUIT_TIMES;
}
