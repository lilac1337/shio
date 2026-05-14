#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

// v100

#define VT100ED2 "\x1b[2J" // erase in display(2), erase entire display, 4 bytes
#define VT100CUP "\x1b[H" // cursor position, 3 bytes
#define VT100CPR "\x1b[6n" // cursor position report, 4 bytes
#define VT100SM25 "\x1b[?25h" // set mode(25), hide cursor cursor, 6 bytes
#define VT100RM25 "\x1b[?25l" // reset mode(25), show cursor, 6 bytes
#define VT100EL "\x1b[K" // erase in line, 3 bytes

// erase in display
static inline int32_t v100ed() { return write(STDOUT_FILENO, "\x1b[2J", 4); }

// cursor position
static inline int32_t v100cup() { return write(STDOUT_FILENO, "\x1b[H", 3); }

// cursor position report
static inline int32_t v100cpr() { return write(STDOUT_FILENO, "\x1b[6n", 4); }

static inline int32_t v100placecursor(int32_t x, int32_t y) {
    char buf[16];

    size_t b = sprintf(buf, "\x1b[%dC\x1b%dB", x, y);
    return write(STDOUT_FILENO, buf, b);
}

void die(const char *s);
void disablerawmode();
void enablerawmode();
int32_t readkey();
int32_t getcursorpos(int32_t *r, int32_t *c);
int32_t getwindowsize(int32_t *r, int32_t *c);

#endif
