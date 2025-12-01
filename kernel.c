#include "sv39.h"
#include <limits.h>
#include <stdint.h>

#define UART ((volatile u8 *)0x10000000)
#define PAGE_SIZE 4096

typedef struct Symbol Symbol;
extern Symbol kernel_start;
extern Symbol kernel_end;

extern Sv39PageTableEntry giga_page_table[4];
extern Sv39PageTableEntry mega_page_tables[4][512];
extern Sv39PageTableEntry page_tables[4][512][512];

#define SATP_FIELDS(x) \
    x(SATP_PPN, 0, 43) \
    x(SATP_ASID, 44, 59) \
    x(SATP_MODE, 60, 63)

SATP_FIELDS(GEN_BIT_FIELD_ACCESS_HELPERS)

enum {
    SATP_MODE_BARE,
    SATP_MODE_SV39 = 8,
};

static void printString(const char *str) {
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

static void printUMax(uMax value, u8 radix, u8 min_width) {
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

static void mmap(Sv39VirtualAddress virtual, Sv39PhysicalAddress physical) {
    u16 vpn2 = SV39_VA_VPN2_Get(virtual);
    Sv39PageTableEntry pte = SV39_PTE_V_MASK;
    pte = SV39_PTE_PPN_Set(pte, (uPtr)(mega_page_tables[vpn2]) / PAGE_SIZE);
    giga_page_table[vpn2] = pte;

    u16 vpn1 = SV39_VA_VPN1_Get(virtual);
    pte = SV39_PTE_V_MASK;
    pte = SV39_PTE_PPN_Set(pte, (uPtr)(page_tables[vpn2][vpn1]) / PAGE_SIZE);
    mega_page_tables[vpn2][vpn1] = pte;

    u16 vpn0 = SV39_VA_VPN0_Get(virtual);
    pte = SV39_PTE_V_MASK | SV39_PTE_R_MASK | SV39_PTE_W_MASK | SV39_PTE_X_MASK;
    pte = SV39_PTE_PPN_Set(pte, (uPtr)(physical / PAGE_SIZE));
    page_tables[vpn2][vpn1][vpn0] = pte;
}

void trap(void) {
    printString("trap\n");
}

void main(uXLEN hart_id, void *fte_header) {
    (void)fte_header;

    for (uPtr addr = (uPtr)&kernel_start; addr < (uPtr)&kernel_end; addr += PAGE_SIZE) {
        mmap(addr, addr);
    }

    mmap((uPtr)UART, (uPtr)UART);

    printString("hello from C!\n");
    printString("running on hart ");
    printUMax(hart_id, 10, 1);
    printString("\n");

    uXLEN satp = 0;
    satp = SATP_PPN_Set(satp, (uPtr)giga_page_table / PAGE_SIZE);
    satp = SATP_MODE_Set(satp, SATP_MODE_SV39);

    __asm__ volatile (
        "csrw satp,%0\n"
        "sfence.vma x0,x0\n"
        : : "r" (satp)
    );

    printString("virtual memory enabled\n");
}
