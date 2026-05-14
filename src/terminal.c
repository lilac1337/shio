#include "terminal.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "shio.h"

// v100

#define VT100ED2 "\x1b[2J" // erase in display(2), erase entire display, 4 bytes
#define VT100CUP "\x1b[H" // cursor position, 3 bytes
#define VT100CPR "\x1b[6n" // cursor position report, 4 bytes
#define VT100SM25 "\x1b[?25h" // set mode(25), hide cursor cursor, 6 bytes
#define VT100RM25 "\x1b[?25l" // reset mode(25), show cursor, 6 bytes
#define VT100EL "\x1b[K" // erase in line, 3 bytes


// terminal

void die(const char *s) {
    v100ed();
    v100cup();
    
    perror(s);
    exit(1);
}

void disablerawmode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &c.origtios) == -1)
        die("tcsetattr failed in disablerawmode");
}

void enablerawmode() {
    struct termios tios;

    if (tcgetattr(STDIN_FILENO, &tios) == -1)
        die("tcgetattr failed in enablerawmode");
    
    c.origtios = tios;

    tios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    tios.c_oflag &= ~(OPOST);
    tios.c_cflag |= ~(CS8);
    tios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios) == -1)
        die("tcsetattr failed in enablerawmode");

    atexit(disablerawmode);
}

int32_t readkey() {
    int32_t nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            die("read in readkey");
    }

    // if char starts with escape seq
    if (c == '\x1b') {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    
        // handle arrow keys & page up/down
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                
                if (seq[2] == '~') {
                    switch (seq[1]) {
                    case '5': return PAGE_UP;
                    case '6': return PAGE_DOWN;
                    }
                }
            }
            
            switch (seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            }
        }
    }

    // emacs-like movement
    switch (c) {
    case CTRL_KEY('p'): return ARROW_UP;
    case CTRL_KEY('n'): return ARROW_DOWN;
    case CTRL_KEY('f'): return ARROW_RIGHT;
    case CTRL_KEY('b'): return ARROW_LEFT;
    }

    return c;
}

int32_t getcursorpos(int32_t *r, int32_t *c) {
    char buf[32];
    uint32_t i = 0;
    
    if (v100cpr() != 4) return -1;

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        ++i;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if (sscanf(&buf[2], "%d;%d", r, c) != 2)
        return -1;

    return 0;
}

int32_t getwindowsize(int32_t *r, int32_t *c) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return getcursorpos(r, c);
    }

    *c = ws.ws_col;
    *r = ws.ws_row;

    return 0;
}
