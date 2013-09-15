TARGET = kernel.elf module.elf

all:
	$(STRIP) $(TARGET)

install:
	./mount
	cp -f kernel.elf floppy
	cp -f module.elf floppy
	./umount
	dd if=bootdisk of=/dev/fd0

clean:
	rm -f $(TARGET)

include ../Makefile.rules
