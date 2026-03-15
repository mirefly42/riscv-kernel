#include "io.h"
#include <limits.h>

void printString(const char *str) {
    for (char c; (c = *str) != '\0'; ++str) {
        UART[0] = c;
    }
}

static char digitChar(u8 digit) {
    if (digit < 10) {
        return '0' + digit;
    }

    return 'A' + (digit - 10);
}

void printUMax(uMax value, u8 radix, u8 min_width) {
    char buf[sizeof(value) * CHAR_BIT];
    char *const end = buf + sizeof(buf);

    char *c_ptr = end;
    while (value > 0) {
        *(--c_ptr) = digitChar(value % radix);
        value /= radix;
    };

    const u8 len = end - c_ptr;
    if (len < min_width) {
        for (u8 i = 0; i < (min_width - len); ++i) {
            UART[0] = digitChar(0);
        }
    }

    for (;c_ptr != end; ++c_ptr) {
        UART[0] = *c_ptr;
    }
}
