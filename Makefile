OBJCOPY = objcopy
LD = ld
AS = as
CC = gcc

HDR = bits.h sv39.h types.h

kernel.bin: kernel.elf
	${CROSS_COMPILE}${OBJCOPY} -O binary kernel.elf $@

kernel.elf: entry.o kernel.o
	${CROSS_COMPILE}${LD} -T link.ld --no-relax entry.o kernel.o -o $@

entry.o: entry.s
	${CROSS_COMPILE}${AS} entry.s -o $@

kernel.o: kernel.c ${HDR}
	${CROSS_COMPILE}${CC} -std=c23 -ffreestanding -fno-pie -mcmodel=medany -mno-relax -c kernel.c -o $@ ${CFLAGS}

clean:
	rm -f kernel.bin kernel.elf entry.o kernel.o

.PHONY: clean
