ROOTDIR = ../../..
include $(ROOTDIR)/Makefile.rules

OBJS =	cons.o \
	kbd.o

TGT = $(LIBDIR)/libdev.a

all: $(OBJS)
	$(AR) $(TGT) $(OBJS)
	$(RANLIB) $(TGT)
clean:
	$(RM) $(OBJS) $(TGT)
