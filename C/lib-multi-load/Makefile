all: helloworld
libfoo.so: foo.c
	$(CC) -o $@ $< -shared -fPIC
helloworld: helloworld.c libfoo.so
	$(CC) -o $@ $< -L . -lfoo
clean:
	$(RM) helloworld libfoo.so
