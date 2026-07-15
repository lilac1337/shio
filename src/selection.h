#ifndef SHIO_SELECTION_H
#define SHIO_SELECTION_H

#include "shio.h"

void resetselection();
void handleselection(u32 scx, u32 ecx, u32 r);
void cutselection();
void pasteselection(u32 cx, u32 cy);
void copyselection();

#endif
