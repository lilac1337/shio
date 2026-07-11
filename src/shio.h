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

#include <stdbool.h>
#include <stddef.h>
#include <termios.h>
#include <time.h>

#include "filetypes.h"
#include "string.h"
#include "types.h"

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define CTRL_KEY(k) ((k) & 0x1f)

#define SHIO_VERSION "0.0.1"
#define SHIO_TAB_STOP 4
#define SHIO_QUIT_TIMES 3

// most of these should probably go in types.h
// or types.h should be removed

typedef struct {
    u32 idx;
    size_t size;
    size_t rsize;
    char *chars;
    char *render;
    unsigned char *hl;
    bool hlmlcmt; // multi-line comment in row
} row;

typedef struct {
    u32 x;
    u32 y;
} cursor;

typedef struct {
    u32 r;
    u32 c;
} wsize;

typedef struct {
    size_t sidx; // these are indexes into the render array
    size_t eidx; // not the char array
    i32 roff;
    u32 r;
} selection;

// this is the same struct as sz, but it's managed manually
// instead of having the helper functions
// --- thinking about it more, we either don't need len
// or don't need the \0 at the end of s; we should remove one
// eventually
typedef struct {
    char *s;
    size_t len;
} cpystr;

typedef struct {
    // 8-byte aligned
    row *r; // row array
    char *fn; // file name
    syntax *syn; // syntax
    cpystr copy;
    selection slctn; // selection
    time_t time;

    // 4-byte aligned
    cursor cur; // cursor pos
    wsize ws; // window size
    u32 roff; // row offset
    u32 coff; // column offset
    u32 nrows; // num rows
    u32 rx; // pos in row
    u32 dirty; // file has been modified
    u32 lno; // line number offset for cur
    bool select; // are we selecting text?

    // cold
    struct termios origtios; // state of termios for exiting
    char status[80];
} config;

extern config c;

void init();

#endif
