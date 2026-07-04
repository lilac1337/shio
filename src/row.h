#ifndef SHIO_ROW_H
#define SHIO_ROW_H

#include "shio.h"

void updatelinenumoffset();
row *getrow(size_t idx);
u32 cx2rx(row *r, u32 cx);
u32 rx2cx(row *r, u32 rx);
void updaterow(row *r);
void insertrow(u32 at, const char *s, size_t len);
void freerow(row *r);
void delrow(u32 at);
void rowinsertchar(row *r, u32 at, i32 ch);
void rowappendstring(row *r, const char *s, size_t len);
void rowdelchar(row *r, u32 at);

#endif
