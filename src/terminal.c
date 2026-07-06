#include "terminal.h"
#include "types.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>

// v100

#define VT100ED2 "\x1b[2J" // erase in display(2), erase entire display, 4 bytes
#define VT100CUP "\x1b[H" // cursor position, 3 bytes
#define VT100CPR "\x1b[6n" // cursor position report, 4 bytes
#define VT100SM25 "\x1b[?25h" // set mode(25), hide cursor cursor, 6 bytes
#define VT100RM25 "\x1b[?25l" // reset mode(25), show cursor, 6 bytes
#define VT100EL "\x1b[K" // erase in line, 3 bytes

[[noreturn]] void die(const char *s) {
    v100ed();
    v100cup();

    if (errno)
        perror(s);
    else
        fprintf(stderr, "%s\n", s);
    
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

    tios.c_iflag &= (tcflag_t)~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    tios.c_oflag &= (tcflag_t)~(OPOST);
    tios.c_cflag |= (tcflag_t)(CS8);
    tios.c_lflag &= (tcflag_t)~(ECHO | ICANON | IEXTEN | ISIG);
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios) == -1)
        die("tcsetattr failed in enablerawmode");

    atexit(disablerawmode);
}

i32 readkey() {
    ssize_t nread;
    char ch;

    while ((nread = read(STDIN_FILENO, &ch, 1ul)) != 1l) {
        if (nread == -1l && errno != EAGAIN)
            die("read in readkey");
    }

    // if char starts with escape seq
    if (ch == '\x1b') {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1l)
            return '\x1b';

        switch (seq[0]) {
        case 'x': return META_X;
        case 'f': return META_F;
        case 'b': return META_B;
        }
        
        if (read(STDIN_FILENO, &seq[1], 1) != 1l)
            return '\x1b';
    
        // handle arrow keys & page up/down
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1ul) != 1l)
                    return '\x1b';

                // these special characters send <esc>[ then a number, and then ~
                // so we check for the ~ and then check which number it was
                if (seq[2] == '~') {
                    switch (seq[1]) {
                    case '1': return HOME_KEY;
                    case '3': return DEL_KEY;
                    case '4': return END_KEY;
                    case '5': return PAGE_UP;
                    case '6': return PAGE_DOWN;
                    case '7': return HOME_KEY;
                    case '8': return END_KEY;
                    }
                }
            }
            
            switch (seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
            }
        }

        if (seq[0] == 'O') {
            switch (seq[1]) {
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
            }
        }

        return '\x1b';
    }

    // emacs-like movement
    switch (ch) {
    case CTRL_KEY('p'): return ARROW_UP;
    case CTRL_KEY('n'): return ARROW_DOWN;
    case CTRL_KEY('f'): return ARROW_RIGHT;
    case CTRL_KEY('b'): return ARROW_LEFT;
    case CTRL_KEY('a'): return HOME_KEY;
    case CTRL_KEY('e'): return END_KEY;
    }

    return ch;
}

errc getcursorpos(wsize *ews) {
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
    if (sscanf(&buf[2], "%" SCNu32 ";%" SCNu32, &ews->r, &ews->c) != 2)
        return -1;

    return 0;
}

errc getwindowsize(wsize *ews) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return getcursorpos(ews);
    }

    ews->c = ws.ws_col;
    ews->r = ws.ws_row;
    
    return 0;
}
