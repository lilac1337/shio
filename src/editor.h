#ifndef SHIO_EDITOR_H
#define SHIO_EDITOR_H

#include "shio.h"

void editorinsertchar(u32 cx, u32 cy, i32 ch, bool at);
void editorinsertnl();
void editordelchar(row *r, u32 cx, u32 cy, bool at);

#endif
