#ifndef SHIO_INPUT_H
#define SHIO_INPUT_H

#include "shio.h"

#include <stdint.h>

typedef void (*callback)(char *, u32);

char *prompt(const char *p, void (*cb)(char *, u32));
void movecursor(int32_t k);
void processkeypress();

#endif
