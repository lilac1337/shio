#include "shio.h"
#include "terminal.h"
#include "output.h"
#include "input.h"
#include "file.h"

config c;

void init() {
    c.cur.x = 0u;
    c.cur.y = 0u;
    c.roff = 0u;
    c.coff = 0u;
    c.nrows = 0u;
    c.rx = 0u;
    c.dirty = 0u;
    c.r = NULL;
    c.fn = NULL;
    c.status[0] = '\0';
    c.time = 0;
    c.syn = NULL;
    
    if (getwindowsize(&c.ws) == -1)
        die("getwindowsize in init");

    c.ws.r -= 2; // make room for status bar
}

int main(int argc, char *argv[]) {
    enablerawmode();
    init();

    if (argc >= 2)
        fileopen(argv[1]);

    setstatus("help: C-x = save, C-q = quit, C-s = search");
    
    while (true) {
        refreshscreen();
        processkeypress();
    }    

    return 0;
}
