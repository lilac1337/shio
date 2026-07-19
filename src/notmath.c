#include "notmath.h"

u32 u32pow(u32 b, u32 e) {
    u32 r = 1u;

    while (e > 0) {
        if (e & 1)
            r *= b;

        b *= b;
        e >>= 1;
    }

    return r;
}
