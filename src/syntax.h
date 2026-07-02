#ifndef SHIO_SYNTAX_H
#define SHIO_SYNTAX_H

#include "shio.h"

#include <stdbool.h>

bool isseparator(int ch);
void updatesyntax(row *r);
u32 syntax2color(u32 hl);
void selectsyntaxhl();

#endif
