#!/usr/bin/env bash

# This should be run inside vagrant, see build.bat

export PREFIX="$HOME/opt/cross/"
export PATH="$PREFIX/bin:$PATH"

cd /vagrant

mkdir -p bin
mkdir -p bin/obj
mkdir -p bin/dis

# boot.s
i686-elf-as src/boot.s -o bin/obj/boot.o
i686-elf-objdump -d bin/obj/boot.o -M intel > bin/dis/boot.s

# interrupts.s
i686-elf-as src/interrupts.s -o bin/obj/interrupts.o
i686-elf-objdump -d bin/obj/interrupts.o -M intel > bin/dis/interrupts.s

# kernel.c
i686-elf-gcc -c src/kernel.c -o bin/obj/kernel.o \
	-std=gnu99 -ffreestanding -O2 -Wall -Wextra \
	-fno-diagnostics-color
i686-elf-objdump -d bin/obj/kernel.o -M intel > bin/dis/kernel.s

# linker.ld
i686-elf-gcc -T src/linker.ld -o bin/roguos.bin \
	-ffreestanding -O2 -nostdlib -lgcc \
	-fno-diagnostics-color \
	bin/obj/boot.o bin/obj/kernel.o bin/obj/interrupts.o

mkdir -p bin/iso/boot/grub
cp bin/roguos.bin bin/iso/boot/roguos.bin
cp grub.cfg bin/iso/boot/grub/grub.cfg
grub-mkrescue -o bin/roguos.iso bin/iso &> /dev/null

