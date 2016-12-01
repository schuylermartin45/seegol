#!/bin/bash

# compile &&
# link &&
# convert ELF to single-section binary format &&
# dd to zero-out floppy image && dd to floppy image
gcc -c -g -Os -m16 -ffreestanding -Wall -Werror -I. -o bin/main.o \
    src/kern/main.c \
&& \
ld -melf_i386 -static -Tboot_linker.ld -nostdlib --nmagic -o bin/main.elf \
    bin/main.o \
&& \
objcopy -O binary bin/main.elf bin/main.b \
&& \
dd if=/dev/zero of=bin/floppy.img bs=1024 count=1440 \
&& \
dd if=bin/main.b of=bin/floppy.img bs=1 count=512 conv=notrunc
