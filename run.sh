#!/bin/sh
exec qemu-system-riscv64 \
    -M virt \
    -nographic \
    -device ramfb \
    -display sdl \
    -kernel kernel.bin \
    "$@"
