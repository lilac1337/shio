#ifndef SHIO_MOVEMENT_H
#define SHIO_MOVEMENT_H

#include "shio.h"

void resetselection();
void handleselection(u32 scx, u32 ecx, u32 r);
void copyselection();
void wordfoward();
void wordbackward();
void movecursor(i32 k);

#endif
