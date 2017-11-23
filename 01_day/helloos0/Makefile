# $@ = target file
# $< = first dependency
# $^ = all dependencies

# First rule is the one executed when no parameters are fed to the Makefile
all: run

# Notice how dependencies are built as needed

run: helloos.img
	qemu-system-i386 -fda $<

clean:
	rm *.bin 
