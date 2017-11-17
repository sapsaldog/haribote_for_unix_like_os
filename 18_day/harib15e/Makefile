CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
OBJCOPY = /usr/local/i386elfgcc/bin/i386-elf-objcopy

TOOL_DIR = ../../tools
INC_DIR = $(TOOL_DIR)/golibc
MAKEFONT = $(TOOL_DIR)/makefont/makefont

CFLAGS = -g -m32 -nostdlib -nostdinc -fno-stack-protector -nostartfiles -nodefaultlibs \
		 -Wall -Wextra
LIB_OPT = -lgolibc

C_SOURCES = $(wildcard *.c)
OBJ = ${C_SOURCES:.c=.o} 
OBJ += naskfunc.o hankaku.o 

os-image.bin : ipl10.bin asmhead.bin haribote.sys
	cat $^ > os-image.bin

haribote.sys: $(OBJ)
	${LD} -T haribote.lds -o $@ $^ --oformat binary $(LIB_OPT)

haribote.elf: $(OBJ)
	${LD} -T haribote.lds -o $@ $^ $(LIB_OPT)

hankaku.bin: hankaku.txt
	${MAKEFONT} $< $@

hankaku.o: hankaku.bin
	${OBJCOPY} --readonly-text -I binary $< -O elf32-i386 $@ -B i386 

asmhead.bin: asmhead.asm
	nasm $< -f bin -o $@

ipl10.bin: ipl10.asm
	nasm $< -f bin -o $@

debug: os-image.bin haribote.elf
	qemu-system-i386 -s -S -fda os-image.bin -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file haribote.elf"

run: os-image.bin
	qemu-system-i386 -fda $<

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -I${INC_DIR} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

clean:
	rm *.bin *.o *.sys *.elf
