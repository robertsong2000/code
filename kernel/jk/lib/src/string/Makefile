ROOTDIR = ../../..
include $(ROOTDIR)/Makefile.rules

OBJS =	memset.o \
	strcat.o \
	strcmp.o \
	strlen.o \
	strncpy.o \
	strtok.o \
	memcpy.o \
	sprintf.o  \
	strchr.o \
	strcpy.o \
	strncmp.o \
	strstr.o

TGT = $(LIBDIR)/libstring.a

all: $(OBJS)
	$(AR) $(TGT) $(OBJS)
	$(RANLIB) $(TGT)
clean:
	$(RM) $(OBJS) $(TGT)
