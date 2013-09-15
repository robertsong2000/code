ROOTDIR = ../..
include $(ROOTDIR)/Makefile.rules


OBJS =	main.o \
	init.o \
	loader.o

TGT = $(ROOTDIR)/boot/module.elf
LIBDIR = $(ROOTDIR)/lib

all: kern build

kern: $(OBJS)

build:
	$(LD) $(LDFLAGS) -nostdlib -L$(LIBDIR) -o $(TGT) $(OBJS) $(LIBS)
	$(SIZE) $(TGT)
			
clean:
	$(RM) $(OBJS) $(TGT)
