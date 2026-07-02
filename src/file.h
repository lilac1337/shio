#ifndef SHIO_FILE_H
#define SHIO_FILE_H

#include "shio.h"

#include <stdbool.h>
#include <sys/types.h>

char *rows2string(ssize_t *buflen, bool trail);
void fileopen(const char *f);
void save();

#endif
