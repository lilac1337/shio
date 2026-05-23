#ifndef SHIO_SHIO_H
#define SHIO_SHIO_H

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stddef.h>
#include <termios.h>

#include "types.h"

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define CTRL_KEY(k) ((k) & 0x1f)

#define SHIO_VERSION "0.0.1"

typedef struct {
    size_t size;
    char *chars;
} row;

typedef struct {
    i32 x;
    i32 y;
} cursor;

typedef struct {
    u32 r;
    u32 c;
} wsize;

typedef struct {
    cursor cur;
    wsize ws;
    u32 roff;
    i32 nrows;
    row *r;
    struct termios origtios;
} config;

extern config c;

#endif
