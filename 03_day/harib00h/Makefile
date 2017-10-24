# $@ = target file
# $< = first dependency
# $^ = all dependencies

# First rule is the one executed when no parameters are fed to the Makefile
all: run

# Notice how dependencies are built as needed
os-image.bin : ipl10.bin haribote.bin
	cat $^ > os-image.bin

haribote.bin: haribote.asm 
	nasm $< -f bin -o $@	

ipl10.bin: ipl10.asm
	nasm $< -f bin -o $@

run: os-image.bin
	qemu-system-i386 -fda $<

clean:
	rm *.bin 
