ROOTDIR = ..
include $(ROOTDIR)/Makefile.rules

# init.o must be first
#
OBJS =	$(ROOTDIR)/arch/$(ARCH)/entry.o \
	$(ROOTDIR)/arch/$(ARCH)/start.o \
	init.o \
	atomic.o \
	internal.o \
	memory.o \
	printf.o \
	spinlock.o \
	timer.o \
	condition.o \
	ioport.o \
	module.o \
	processor.o \
	thread.o \
	irq.o \
	mutex.o \
	rwlock.o \
	time.o 
	
LIBS = -lgcc 

TGT = $(ROOTDIR)/boot/kernel.elf

all: kern build

kern: $(OBJS)

build:
	$(CC) $(LD_FLAGS) $(ELF_LINKER_FLAGS) -L$(LIBDIR) \
	-o $(TGT) $(OBJS) $(LIBS)
	$(SIZE) $(TGT)

clean: kernclean

kernclean:
	$(RM) $(OBJS) $(TGT)

