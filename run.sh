#!/bin/sh
exec qemu-system-riscv64 -M virt -nographic -kernel kernel.bin "$@"
