TOPDIR = ../..

all:
	$(ASM) exec_library.asm -o bin/exec_library.bin
	$(ASM) floppy_device.asm -o bin/floppy_device.bin
	$(ASM) fat12_handler.asm -o bin/fat12_handler.bin
	$(ASM) console_library.asm -o bin/console_library.bin
	$(ASM) keyboard_handler.asm -o bin/keyboard_handler.bin
	$(ASM) init.asm -o bin/init.bin

clean:
	rm -f bin/*

include $(TOPDIR)/assembler.inc
