#include "shio.h"
#include "terminal.h"
#include "output.h"
#include "input.h"

config c;

void init() {
    c.cx = 0;
    c.cy = 0;
    
    if (getwindowsize(&c.sr, &c.sc) == -1)
        die("getwindowsize in init");
}

int main(int argc, char **argv) {
    enablerawmode();
    init();

    while  (1) {
        refreshscreen();
        processkeypress();
    }

    return 0;
}
