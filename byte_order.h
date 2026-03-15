#ifndef BYTE_ORDER_H
#define BYTE_ORDER_H

#include "types.h"

static uMax byteSwap(uMax value, u8 size) {
    uMax result = 0;
    for (u8 i = 0; i < size; ++i) {
        result <<= 8;
        result |= (u8)value;
        value >>= 8;
    }
    return result;
}

#define BE_GEN(bits) \
    typedef struct U##bits##Be { \
        u##bits data; \
    } U##bits##Be; \
    \
    static inline U##bits##Be u##bits##ToBe(u##bits value) { \
        return (U##bits##Be){byteSwap(value, sizeof(value))}; \
    } \
    \
    static inline u##bits u##bits##FromBe(U##bits##Be value) { \
        return byteSwap(value.data, sizeof(value.data)); \
    }

BE_GEN(16)
BE_GEN(32)
BE_GEN(64)

#endif
