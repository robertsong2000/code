TOPDIR = ..

all:
	$(ASM) shell.asm -o bin/shell.bin
	
	$(ASM) dump_impl.asm -o bin/dump_impl.bin
	$(ASM) dump.asm -o bin/dump.bin
	$(ASM) hello_impl.asm -o bin/hello_impl.bin
	$(ASM) hello.asm -o bin/hello.bin

clean:
	rm -f bin/*

include $(TOPDIR)/assembler.inc                                                                    
