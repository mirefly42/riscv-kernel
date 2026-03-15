.option norelax

.equ STACK_SIZE,1024*1024
.equ PAGE_SIZE,4096

.section .text

# setup stack
mv fp,x0
la sp,stack_base

la t0,trap_handler
csrw stvec,t0

jal main

halt:
    j halt

.globl storeU64
storeU64:
    sd a1,0(a0)
    ret

trap_handler:
    jal trap
    bnez a0,halt
    sret

.section .rodata
.balign PAGE_SIZE
.globl user_main
user_main:
    li a0,0
    ecall

    li a0,42
    ecall

    li a0,1
    ecall

.section .bss
.balign PAGE_SIZE
.globl giga_page_table
giga_page_table:
.zero 4*8

.balign PAGE_SIZE
.globl mega_page_tables
mega_page_tables:
.zero 4*512*8

.balign PAGE_SIZE
.globl page_tables
page_tables:
.zero 4*512*512*8

.balign PAGE_SIZE
.skip STACK_SIZE
stack_base:
