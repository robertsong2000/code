IMAGE_NAME = orzmicro.img
MKIMG = tools/mkorzimg

all:
	$(MAKE) -C tools
	$(MAKE) -C boot
	$(MAKE) -C kernel
	$(MAKE) -C apps
	
	$(MKIMG) b boot/boot.bin $(IMAGE_NAME)
	$(MKIMG) a kernel/orzkernl ORZKERNL $(IMAGE_NAME)
	$(MKIMG) a apps/bin/shell.bin SHELL $(IMAGE_NAME)
	
	$(MKIMG) a apps/bin/dump.bin DUMP $(IMAGE_NAME)
	$(MKIMG) a apps/bin/hello.bin HELLO $(IMAGE_NAME)

clean:
	$(MAKE) -C tools clean
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	$(MAKE) -C apps clean
	rm -f $(IMAGE_NAME)
