#ifndef SHIO_COMMANDS_H
#define SHIO_COMMANDS_H

#include "shio.h"
typedef void (*ccallback)();

typedef struct {
    const char *c;
    ccallback func;
} command;

void commandcallback(char *q, u32 k);
void executecommand();

// ccallback functions
void chelp();
void csave();

extern const command commands[];
extern const size_t commandcnt;

#endif
