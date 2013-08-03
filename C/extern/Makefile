target=extern

$(target): $(target).c foo.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(target)
