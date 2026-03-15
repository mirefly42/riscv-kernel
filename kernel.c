#include "io.h"
#include "sv39.h"

#define PAGE_SIZE 4096

extern void testRamFb(void);

typedef struct Symbol Symbol;
extern Symbol kernel_start;
extern Symbol kernel_end;
extern Symbol user_main;

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

static void mmap(Sv39VirtualAddress virtual, Sv39PhysicalAddress physical, u8 flags) {
    u16 vpn2 = SV39_VA_VPN2_Get(virtual);
    Sv39PageTableEntry pte = SV39_PTE_V_MASK;
    pte = SV39_PTE_PPN_Set(pte, (uPtr)(mega_page_tables[vpn2]) / PAGE_SIZE);
    giga_page_table[vpn2] = pte;

    u16 vpn1 = SV39_VA_VPN1_Get(virtual);
    pte = SV39_PTE_V_MASK;
    pte = SV39_PTE_PPN_Set(pte, (uPtr)(page_tables[vpn2][vpn1]) / PAGE_SIZE);
    mega_page_tables[vpn2][vpn1] = pte;

    u16 vpn0 = SV39_VA_VPN0_Get(virtual);
    pte = SV39_PTE_V_MASK | flags;
    pte = SV39_PTE_PPN_Set(pte, (uPtr)(physical / PAGE_SIZE));
    page_tables[vpn2][vpn1][vpn0] = pte;
}

uXLEN trap(uXLEN syscall_id) {
    printString("trap ");
    uXLEN cause;
    __asm__ volatile ("csrr %0,scause" : "=r" (cause));
    printUMax(cause, 10, 1);
    printString("\n");

    if (cause == 8) {
        __asm__ volatile (
            "csrr t0,sepc\n"
            "addi t0,t0,4\n"
            "csrw sepc,t0\n"
            : : : "t0"
        );

        printString("syscall ");
        printUMax(syscall_id, 10, 1);
        printString(" executed!\n");

        switch (syscall_id) {
            case 1:
                printString("exit\n");
                return 1;
        }
    }

    return 0;
}

void main(uXLEN hart_id, void *fte_header) {
    (void)fte_header;

    const u8 pte_flags_rwx = SV39_PTE_R_MASK | SV39_PTE_W_MASK | SV39_PTE_X_MASK;

    for (uPtr addr = (uPtr)&kernel_start; addr < (uPtr)&kernel_end; addr += PAGE_SIZE) {
        mmap(addr, addr, pte_flags_rwx);
    }

    mmap((uPtr)UART, (uPtr)UART, pte_flags_rwx);

    printString("hello from C!\n");
    printString("running on hart ");
    printUMax(hart_id, 10, 1);
    printString("\n");

    testRamFb();

    uXLEN satp = 0;
    satp = SATP_PPN_Set(satp, (uPtr)giga_page_table / PAGE_SIZE);
    satp = SATP_MODE_Set(satp, SATP_MODE_SV39);

    mmap(PAGE_SIZE, (uPtr)&user_main, pte_flags_rwx | SV39_PTE_U_MASK);

    printString("switching to U-mode\n");

    __asm__ volatile (
        "csrw satp,%0\n"
        "sfence.vma x0,x0\n"
        "li t0,4096\n"
        "csrw sepc,t0\n"
        "\n"
        "li t0,1<<8\n"
        "csrc sstatus,t0\n"
        "sret\n"
        : : "r" (satp)
    );
}
