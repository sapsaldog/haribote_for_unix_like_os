INCPATH  = .
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
AR = /usr/local/i386elfgcc/bin/i386-elf-ar

TARGET     = libgolibc
OBJS       := abs.o atof.o atoi.o errno.o frexp.o ldexp.o \
	memchr.o memcmp.o memmove.o memset.o qsort.o rand.o \
	sprintf.o strcat.o strcmp.o strcpy.o strcspn.o strdup.o \
	strlen.o strncat.o strncmp.o strncpy.o strpbrk.o strrchr.o \
	strspn.o strstr.o strtol.o strtoul.o strtoul0.o vsprintf.o

all:
	$(MAKE) $(TARGET).a

$(TARGET).a : $(OBJS)
	$(AR) rcs $@ $(OBJS) $<

clean:
	rm -rf *.o $(TARGET).a

%.o: %.c
	${CC} ${CFLAGS} -I$(INCPATH) -ffreestanding -fno-asynchronous-unwind-tables -c $< -o $@

