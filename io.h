#ifndef IO_H
#define IO_H

#include "types.h"

#define UART ((volatile u8 *)0x10000000)

void printString(const char *str);
void printUMax(uMax value, u8 radix, u8 min_width);

#endif
