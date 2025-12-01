#ifndef BITS_H
#define BITS_H

#include "types.h"

#define HIGH_BIT(i) (1ull << (i))
#define HIGH_BITS(i, n) (((1ull << (n)) - 1) << (i))
#define HIGH_BITS_RANGE(a, b) HIGH_BITS((a), (b) + 1 - (a)) // inclusive

#define GEN_BIT_FIELD_ACCESS_HELPERS(name, from, to) \
    static constexpr uMax name##_MASK = HIGH_BITS_RANGE(from, to); \
    \
    static inline uMax name##_Get(uMax bits) { \
        return (bits >> (from)) & HIGH_BITS(0, (to) - (from) + 1); \
    } \
    \
    [[nodiscard]] static inline uMax name##_Set(uMax bits, uMax value) { \
        bits &= ~name##_MASK; \
        bits |= (value << (from)) & name##_MASK; \
        return bits; \
    } \

#endif
