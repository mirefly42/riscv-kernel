.equ STACK_SIZE,1024*1024
.equ UART,0x10000000

.macro push reg
    addi sp,sp,-8
    sd \reg,0(sp)
.endm

.macro pop reg
    ld \reg,0(sp)
    addi sp,sp,8
.endm

.macro prologue
    push ra
    push fp
    mv fp,sp
.endm

.macro epilogue
    mv sp,fp
    pop fp
    pop ra
    ret
.endm

.section .text

main:
    # setup stack
    la sp,stack_base

    la a0,msg
    jal print_string

    jal echo

    j halt

print_fdt:
    push a0

    addi a0,a0,4
    jal load_be32

    mv a1,a0
    pop a0
    j print_data

load_be32:
    lbu t0,0(a0)
    slli t0,t0,24

    lbu t1,1(a0)
    slli t1,t1,16
    or t0,t0,t1

    lbu t1,2(a0)
    slli t1,t1,8
    or t0,t0,t1

    lbu t1,3(a0)
    or t0,t0,t1

    mv a0,t0
    ret

echo:
    li t0,UART

1: # loop until data available
    lbu t1,5(t0)
    andi t1,t1,1
    beqz t1,1b

    la a0,msg1
    jal print_string

    lbu a0,0(t0)
    jal print_char

    li a0,'\n'
    jal print_char

    j echo

print_string:
    prologue
1:
    push a0
    lbu a0,0(a0)
    beqz a0,2f

    jal print_char
    pop a0
    addi a0,a0,1

    j 1b

2:
    epilogue

dump_memory:
    prologue

1:
    beqz a1,2f

    push a0
    push a1
    lbu a0,0(a0)
    jal print_byte
    pop a1
    pop a0
    addi a0,a0,1
    addi a1,a1,-1

    j 1b

2:
    li a0,'\n'
    jal print_char

    epilogue

print_data:
    prologue

1:
    beqz a1,2f

    push a0
    push a1
    lbu a0,0(a0)
    jal print_char
    pop a1
    pop a0
    addi a0,a0,1
    addi a1,a1,-1

    j 1b

2:
    li a0,'\n'
    jal print_char

    epilogue

print_byte:
    prologue

    push a0
    srli a0,a0,4
    andi a0,a0,0xF
    jal hex_digit
    jal print_char

    pop a0
    andi a0,a0,0xF
    jal hex_digit
    jal print_char

    epilogue

print_char:
    li t0,UART
    sb a0,0(t0)
    ret

dec_digit:
    addi a0,a0,'0'
    ret

hex_digit:
    li t0,0xA
    bgeu a0,t0,1f

    j dec_digit
1:
    addi a0,a0,'A' - 0xA
    ret

halt:
    j halt

.section .data
msg: .string "hello\n"
msg1: .string "you entered: "

.section .bss
.balign 8
.skip STACK_SIZE
stack_base:
