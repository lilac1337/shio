#include "commands.h"

#include "input.h"
#include "file.h"
#include "output.h"

#include <stdlib.h>
#include <string.h>

const command commands[] = {{"save", csave}};
const size_t commandcnt = sizeof(commands) / sizeof(commands[0]);

void commandcallback(char *q, u32 k) {
    if (k == '\r') {
        size_t i;
        for (i = 0; i < commandcnt; ++i) {
            if (strcmp(q, commands[i].c) == 0) {
                commands[i].func();
                return;
            }
        }
        
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

void csave() {
    save();
}
