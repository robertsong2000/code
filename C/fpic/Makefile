target1=libnofpic.so
target2=libfpic.so
CFLAGS=-shared -save-temps

all: $(target1) $(target2)

$(target1): fpic.c
	$(CC) $(CFLAGS) -o $@ $<

$(target2): fpic.c
	$(CC) $(CFLAGS) -fPIC -o $@ $<
clean:
	$(RM) $(target1) $(target2)
