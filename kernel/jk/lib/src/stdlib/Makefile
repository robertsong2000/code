ROOTDIR = ../../..
include $(ROOTDIR)/Makefile.rules

OBJS = malloc.o free.o
TGT = $(LIBDIR)/libstdlib.a

all: $(OBJS)
	$(AR) $(TGT) $(OBJS)
	$(RANLIB) $(TGT)
clean:
	$(RM) $(OBJS) $(TGT)
