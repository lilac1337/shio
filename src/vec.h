#ifndef SHIO_VEC_H
#define SHIO_VEC_H

#include "types.h"

#include <stddef.h>

typedef struct {
    u32 *d;
    size_t n;
    size_t c;
} vu32;

vu32 vu32init(void);
void vu32push(vu32 *v, u32 n);
void vu32pop(vu32 *v, size_t n);
u32 vu32at(vu32 *v, size_t n);
void vu32clear(vu32 *v);
void vu32free(vu32 *v);

#endif
