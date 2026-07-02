#ifndef SHIO_OUTPUT_H
#define SHIO_OUTPUT_H

#include "shio.h"
#include "string.h"

void scroll();
void drawrows(sv *txt);
void drawstatus(sv *txt);
void drawmessage(sv *txt);
void refreshscreen();
void setstatus(const char *fmt, ...);

#endif
