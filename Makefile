OBJCOPY = objcopy
LD = ld
AS = as
CC = gcc

SRC = io.c kernel.c qemu_ramfb.c
OBJ = entry.o ${SRC:.c=.o}
HDR = bits.h byte_order.h io.h sv39.h types.h

kernel.bin: kernel.elf
	${CROSS_COMPILE}${OBJCOPY} -O binary kernel.elf $@

kernel.elf: ${OBJ}
	${CROSS_COMPILE}${LD} -T link.ld --no-relax ${OBJ} -o $@

entry.o: entry.s
	${CROSS_COMPILE}${AS} entry.s -o $@

.c.o:
	${CROSS_COMPILE}${CC} -c -std=c23 -ffreestanding -fno-pie -mcmodel=medany -mno-relax -o $@ $< ${CFLAGS}

${OBJ}: ${HDR}

clean:
	rm -f kernel.bin kernel.elf ${OBJ}

.PHONY: clean
