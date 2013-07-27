TOPDIR = ..

all:
	$(MAKE) -C tasks
	$(ASM) main.asm -o orzkernl

clean:
	$(MAKE) -C tasks clean
	rm -f orzkernl

include $(TOPDIR)/assembler.inc
