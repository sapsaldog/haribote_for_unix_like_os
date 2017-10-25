# $@ = target file
# $< = first dependency
# $^ = all dependencies

# First rule is the one executed when no parameters are fed to the Makefile
all: run

# Notice how dependencies are built as needed
ipl.bin: ipl.asm
	nasm $< -f bin -o $@

run: ipl.bin
	qemu-system-i386 -fda $<

clean:
	rm *.bin 
