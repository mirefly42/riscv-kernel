. = 0x80200000; /* OpenSBI under qemu loads the kernel at this address */

PROVIDE(kernel_start = .);
SECTIONS {
    .text ALIGN(4096) : {
        *(.text)
    }
    .data ALIGN(4096) : {
        *(.data)
    }
    .rodata ALIGN(4096) : {
        *(.rodata)
    }
    .bss ALIGN(4096) : {
        *(.bss)
    }
}
PROVIDE(kernel_end = .);
