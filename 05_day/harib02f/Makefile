C_SOURCES = $(wildcard *.c)

OBJ = ${C_SOURCES:.c=.o} 
# modified to use makefile from harib02e
# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
BIN2OBJ = /usr/local/i386elfgcc/bin/i386-elf-objcopy
MAKEFONT = ../../tools/build/makefont
# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		 -Wall -Wextra

# Notice how dependencies are built as needed

os-image.bin : ipl10.bin asmhead.bin haribote.sys
	cat $^ > os-image.bin

haribote.sys: naskfunc.o bootpack.o hankaku.o
	i386-elf-ld -o $@ -Ttext 0x00280000 $^ --oformat binary

# Used for debugging purposes
haribote.elf: naskfunc.o bootpack.o hankaku.o
	i386-elf-ld -o $@ -Ttext 0x00280000 $^

hankaku.bin: hankaku.txt
	${MAKEFONT} $< $@

hankaku_need_to_redefine_symbol_name.o: hankaku.bin
	${BIN2OBJ} -I binary $< -O elf32-i386 $@ -B i386 

hankaku.o: hankaku_need_to_redefine_symbol_name.o
	${BIN2OBJ} -I elf32-i386 $< -O elf32-i386 $@ --redefine-sym _binary_hankaku_bin_start=hankaku

asmhead.bin: asmhead.asm
	nasm $< -f bin -o $@

ipl10.bin: ipl10.asm
	nasm $< -f bin -o $@

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin haribote.elf
	qemu-system-i386 -s -S -fda os-image.bin -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file haribote.elf"

run: os-image.bin
	qemu-system-i386 -fda $<

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

clean:
	rm *.bin *.o *.sys *.elf
