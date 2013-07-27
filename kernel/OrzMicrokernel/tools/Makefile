CC = gcc
CFLAGS = -Os -Wall

BIN_NAME = mkorzimg

# A nifty macro to make testing gcc features easier, but note that everything
# that uses this _must_ use := or it will be re-evaluated for every file.
check_gcc=$(shell \
	if [ "$(1)" != "" ]; then \
		if $(CC) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; \
		then echo "$(1)"; else echo "$(2)"; fi \
	fi)

all:
	$(CC) $(CFLAGS) $(call check_gcc,--param ssp-buffer-size=1474560,) \
		-o $(BIN_NAME) mkorzimg.c

clean:
	rm -f $(BIN_NAME)
