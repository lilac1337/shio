#ifndef SHIO_INPUT_H
#define SHIO_INPUT_H

#include "shio.h"

#include <stdint.h>

enum keys {
    ARROW_LEFT = 0xB00B1E5,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

void movecursor(int32_t k);
void processkeypress();

#endif
