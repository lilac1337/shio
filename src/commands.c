#include "commands.h"

#include "input.h"
#include "file.h"
#include "output.h"

#include "stdlib.h"
#include "string.h"

void commandcallback(char *q, u32 k) {
    if (strcmp(q, "save") == 0 && k == '\r') {
        save();
        return;
    }

    if (k == '\r') {
        setstatus("M-x \"%s\" command not found", q);
        return;
    }

    if (k == '\x1b' || k == CTRL_KEY('g')) {
        setstatus("M-x cancelled");
        return;
    }
        
}

void executecommand() {
    char *q = prompt("M-x: %s (C-g to cancel)", commandcallback);

    if (q) {
        free(q);

        return;
    }
}
