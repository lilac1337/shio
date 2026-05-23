#include "shio.h"
#include "terminal.h"
#include "output.h"
#include "input.h"
#include "file.h"

config c;

void init() {
    c.cur.x = 0;
    c.cur.y = 0;
    c.roff = 0;
    c.nrows = 0;
    c.r = NULL;
    
    if (getwindowsize(&c.ws) == -1)
        die("getwindowsize in init");
}

int main(int argc, char *argv[]) {
    enablerawmode();
    init();

    if (argc >= 2)
        open(argv[1]);

    while  (1) {
        refreshscreen();
        processkeypress();
    }    

    return 0;
}
