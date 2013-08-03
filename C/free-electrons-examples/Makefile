ifneq ($(KERNELRELEASE),)
	obj-m += hello.o
	obj-m += hello_param.o
else
	KDIR := /usr/src/linux-3.0-ARCH
all:
	$(MAKE) -C $(KDIR) M=`pwd` modules
clean:
	make -C $(KDIR) M=$(PWD) clean
	$(RM) modules.order
endif

