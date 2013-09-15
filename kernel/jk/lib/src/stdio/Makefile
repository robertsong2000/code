ROOTDIR = ../../..
include $(ROOTDIR)/Makefile.rules

OBJS = printf.o
TGT = $(LIBDIR)/libstdio.a

all: $(OBJS)
	$(AR) $(TGT) $(OBJS)
	$(RANLIB) $(TGT)
clean:
	$(RM) $(OBJS) $(TGT)
