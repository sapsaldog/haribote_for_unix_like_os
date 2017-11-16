# $@ = target file
# $< = first dependency
# $^ = all dependencies

# First rule is the one executed when no parameters are fed to the 


DEL = rm -rf

all: run

ipl.bin: ipl.asm
	nasm $< -f bin -o $@

haribote.img: ipl.bin
	mformat -C -f 1440 -v HARIBOTE -i haribote.img -B ipl.bin ::

run: haribote.img
	qemu-system-i386 -fda $<

clean :
	-$(DEL) *.bin
	-$(DEL) *.img

