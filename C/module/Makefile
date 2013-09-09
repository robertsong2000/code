obj-m += helloworld.o
helloworld-objs := foo.o test.o

INSTALLDIR ?= /tftpboot
KERNELDIR ?= /home/Kernel

all:
	make -C $(KERNELDIR) M=$(PWD) modules
 
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	$(RM) modules.order

install: all
	mkdir -p $(INSTALLDIR)
	cp helloworld.ko $(INSTALLDIR)
