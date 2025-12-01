#ifndef SV39_H
#define SV39_H

#include "bits.h"
#include "types.h"

typedef u64 Sv39VirtualAddress;
typedef u64 Sv39PhysicalAddress;
typedef u64 Sv39PageTableEntry;

#define SV39_VIRTUAL_ADDRESS_FIELDS(x) \
    x(SV39_VA_PGOFF, 0, 11) \
    x(SV39_VA_VPN0, 12, 20) \
    x(SV39_VA_VPN1, 21, 29) \
    x(SV39_VA_VPN2, 30, 38) \
    x(SV39_VA_VPN, 12, 38)

SV39_VIRTUAL_ADDRESS_FIELDS(GEN_BIT_FIELD_ACCESS_HELPERS)

#define SV39_PHYSICAL_ADDRESS_FIELDS(x) \
    x(SV39_PA_PGOFF, 0, 11) \
    x(SV39_PA_PPN0, 12, 20) \
    x(SV39_PA_PPN1, 21, 29) \
    x(SV39_PA_PPN2, 30, 55) \
    x(SV39_PA_PPN, 12, 55)

SV39_PHYSICAL_ADDRESS_FIELDS(GEN_BIT_FIELD_ACCESS_HELPERS)

#define SV39_PAGE_TABLE_ENTRY_FIELDS(x) \
    x(SV39_PTE_V, 0, 0) \
    x(SV39_PTE_R, 1, 1) \
    x(SV39_PTE_W, 2, 2) \
    x(SV39_PTE_X, 3, 3) \
    x(SV39_PTE_U, 4, 4) \
    x(SV39_PTE_G, 5, 5) \
    x(SV39_PTE_A, 6, 6) \
    x(SV39_PTE_D, 7, 7) \
    x(SV39_PTE_RSW, 8, 9) \
    x(SV39_PTE_PPN0, 10, 18) \
    x(SV39_PTE_PPN1, 19, 27) \
    x(SV39_PTE_PPN2, 28, 53) \
    x(SV39_PTE_PPN, 10, 53) \
    x(SV39_PTE_RESERVED, 54, 60) \
    x(SV39_PTE_PBMT, 61, 62) \
    x(SV39_PTE_N, 63, 63)

SV39_PAGE_TABLE_ENTRY_FIELDS(GEN_BIT_FIELD_ACCESS_HELPERS)

#endif
