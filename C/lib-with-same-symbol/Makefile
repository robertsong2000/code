all: helloworld
libbar.so: bar.c
	$(CC) -o $@ $< -shared -fPIC
libfoo.so: foo.c
	$(CC) -o $@ $< -shared -fPIC
helloworld: helloworld.c libbar.so libfoo.so
	$(CC) -o $@ $< -L . -lbar -lfoo
clean:
	$(RM) helloworld libfoo.so libbar.so
