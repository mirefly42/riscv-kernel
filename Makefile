OBJCOPY = objcopy
LD = ld
AS = as
CC = gcc

kernel.bin: kernel.elf
	${CROSS_COMPILE}${OBJCOPY} -O binary kernel.elf $@

kernel.elf: kernel.o test.o
	${CROSS_COMPILE}${LD} -T link.ld --no-relax kernel.o test.o -o $@

kernel.o: kernel.S
	${CROSS_COMPILE}${AS} kernel.S -o $@

test.o: test.c
	${CROSS_COMPILE}${CC} -ffreestanding -mcmodel=medany -mno-relax -c test.c -o $@

clean:
	rm -f kernel.bin kernel.elf kernel.o test.o

.PHONY: clean
