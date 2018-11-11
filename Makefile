TARGET = FemtoOS.bin
ISOFILE = FemtoOS.iso
CC = /opt/cross/bin/i686-elf-gcc
CFLAGS = -Isrc/include/ -Isrc/include/libc/ -include "stddef.h" -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T linker.ld -ffreestanding -nostdlib -O2

.PHONY: run

SOURCEDIR = src/
BUILDDIR = objects
SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))
SOURCES_ASM := $(shell find $(SOURCEDIR) -name '*.asm')
OBJECTS_ASM := $(addprefix $(BUILDDIR)/,$(SOURCES_ASM:%.asm=%.o))

$(BUILDDIR)/%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) 

$(BUILDDIR)/%.o: %.asm
	nasm -f elf32 $< -o $@

$(TARGET): $(OBJECTS) $(OBJECTS_ASM)
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECTS_ASM) -o $@ -lgcc

$(ISOFILE): $(TARGET)
	-rm isofiles/boot/FemtoOS.bin
	cp FemtoOS.bin isofiles/boot/FemtoOS.bin
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o FemtoOS.iso isofiles/

run: $(ISOFILE)
	qemu-system-x86_64 -boot d -cdrom $(ISOFILE) -m 256

clean:
	-rm -f $(OBJECTS)
	-rm -f $(OBJECTS_ASM)
	-rm -f $(TARGET)
	-rm -f $(ISOFILE)