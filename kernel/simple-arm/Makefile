CROSS_COMPILE := arm-none-linux-gnueabi-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

CFLAGS=-ansi -pedantic -Wall -Wextra -march=armv5te -msoft-float -fPIC -mapcs-frame -marm -ffreestanding -g
LDFLAGS=-N -Ttext=0x10000

kernel.elf: bootstrap.o kernel.o context_switch.o syscalls.o

.PHONY: clean

clean:
	$(RM) *.elf *.o

.SUFFIXES: .s .o .elf

.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^

.o.elf:
	$(LD) $(LDFLAGS) -o $@ $^
