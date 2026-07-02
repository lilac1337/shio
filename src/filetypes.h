#ifndef SHIO_FILETYPE_H
#define SHIO_FILETYPE_H

#include "types.h"

#include <stddef.h>

enum hlflags : u32 {
    HLF_NONE = 0,
    HLF_NUMBERS = (1 << 0),
    HLF_STRINGS = (1 << 1)
};

typedef struct {
    const char *ft;
    const char **fm;
    const char **kws; // keywords
    const char *ss; // special start
    const char *slcmtst; // single-line comment start
    const char *mlcmtst; // multi-line comment start
    const char *mlcmtend;
    u32 flags;
} syntax;

extern const char *c_hl_ext[];
extern const char *c_hl_kws[];
extern syntax hldb[];
extern const size_t hldb_entries;

#endif
