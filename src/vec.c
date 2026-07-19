#include "vec.h"

#include <stdlib.h>

vu32 vu32init(void) {
    vu32 v;

    v.d = NULL;
    v.n = 0ul;
    v.c = 0ul;

    return v;
}

void vu32push(vu32 *v, u32 n) {
    if (v->n >= v->c) {
        v->c = (v->c) ? v->c * 2ul : 64ul;
        v->d = realloc(v->d, v->c * sizeof(u32));
    }

    v->d[v->n++] = n;

    return;
}

void vu32pop(vu32 *v, size_t n) {
    size_t i;
    
    if (!v->n)
        return;

    for (i = 0; i < n; ++i) {
        v->d[v->n] = 0u;
        v->n--;
    }

    return;
}

u32 vu32at(vu32 *v, size_t n) {
    if (n > v->n)
        return 0u;
    
    return v->d[n];
}

void vu32clear(vu32 *v) {
    v->n = 0ul;
}

void vu32free(vu32 *v) {
    free(v->d);
    v->d = NULL;
    v->n = 0ul;
    v->c = 0ul;

    return;
}
