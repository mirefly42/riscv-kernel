. = 0x80200000; /* OpenSBI under qemu loads the kernel at this address */

PROVIDE(kernel_start = .);
SECTIONS {
    .text BLOCK(4096) : {
        *(.text)
    }
    .data BLOCK(4096) : {
        *(.data)
    }
    .rodata BLOCK(4096) : {
        *(.rodata)
    }
    .bss BLOCK(4096) : {
        *(.bss)
    }
}
PROVIDE(kernel_end = .);
