#ifndef SHIO_TERMINAL_H
#define SHIO_TERMINAL_H

#include "shio.h"

#include <stdio.h>
#include <unistd.h>

// v100

#define VT100ED2 "\x1b[2J" // erase in display(2), erase entire display, 4 bytes
#define VT100CUP "\x1b[H" // cursor position, 3 bytes
#define VT100CPR "\x1b[6n" // cursor position report, 4 bytes
#define VT100SM25 "\x1b[?25h" // set mode(25), hide cursor cursor, 6 bytes
#define VT100RM25 "\x1b[?25l" // reset mode(25), show cursor, 6 bytes
#define VT100EL "\x1b[K" // erase in line, 3 bytes
#define VT100SGR "\x1b[m" // sgr reset, 3 bytes
#define VT100SGR7 "\x1b[7m" // sgr negative, 4 bytes
#define VT100COLORDEFAULT "\x1b[39m" // sgr default color, 5 bytes
#define VT100COLORRED "\x1b[31m" // sgr red color, 5 bytes

// erase in display
static inline ssize_t v100ed() { return write(STDOUT_FILENO, "\x1b[2J", 4); }

// cursor position
static inline ssize_t v100cup() { return write(STDOUT_FILENO, "\x1b[H", 3); }

// cursor position report
static inline ssize_t v100cpr() { return write(STDOUT_FILENO, "\x1b[6n", 4); }

void die(const char *s);
void disablerawmode();
void enablerawmode();
i32 readkey();
errc getcursorpos(wsize *ews);
errc getwindowsize(wsize *ews);

static inline ssize_t v100placecursor(i32 x, i32 y) {
    char buf[16];

    ssize_t b = sprintf(buf, "\x1b[%dC\x1b%dB", x, y);

    if (b <= 0)
        die("sprintf in v100placecursor");
    
    return write(STDOUT_FILENO, buf, (size_t)b);
}

#endif
