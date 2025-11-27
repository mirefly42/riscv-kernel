OBJCOPY = objcopy
LD = ld
AS = as

kernel.bin: kernel.elf
	${CROSS_COMPILE}${OBJCOPY} -O binary kernel.elf $@

kernel.elf: kernel.o
	${CROSS_COMPILE}${LD} -T link.ld kernel.o -o $@

kernel.o: kernel.S
	${CROSS_COMPILE}${AS} kernel.S -o $@

clean:
	rm -f kernel.bin kernel.elf kernel.o

.PHONY: clean
