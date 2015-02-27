target=multi-thread
CFLAGS=-g3

$(target): $(target).c
	$(CC) $(CFLAGS) -o $@ $< -lpthread

clean:
	$(RM) $(target)
