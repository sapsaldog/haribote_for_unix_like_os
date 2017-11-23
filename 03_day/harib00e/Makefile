# $@ = target file
# $< = first dependency
# $^ = all dependencies

# First rule is the one executed when no parameters are fed to the Makefile
all: run

# Notice how dependencies are built as needed
os-image.bin : ipl.bin haribote.bin
	cat $^ > os-image.bin

haribote.bin: haribote.asm 
	nasm $< -f bin -o $@	

ipl.bin: ipl.asm
	nasm $< -f bin -o $@

run: os-image.bin
	qemu-system-i386 -fda $<

clean:
	rm *.bin 
