all: bar.so foo.so
bar.so: bar.c
	$(CC) -o $@ $< -shared -fPIC
foo.so: foo.c
	$(CC) -o $@ $< -shared -fPIC
clean:
	$(RM) helloworld bar.so foo.so
