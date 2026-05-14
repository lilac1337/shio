#ifndef SHIO_H
#define SHIO_H

#include <termios.h>

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define CTRL_KEY(k) ((k) & 0x1f)

#define SHIO_VERSION "0.0.1"

enum keys {
    ARROW_LEFT = 0xB00B1E5,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    PAGE_DOWN
};

typedef struct {
    int cx, cy;
    int sr;
    int sc;
    struct termios origtios;
} config;

extern config c;

#endif
