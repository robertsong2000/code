target=float
CFLAGS=-mfloat-abi=softfp -march=armv7-a -mfpu=vfpv3-d16
CFLAGS=-mfloat-abi=softfp -march=armv7-a -mfpu=vfp
CFLAGS=-mfloat-abi=softfp -march=armv7-a -mfpu=vfp -mthumb
CFLAGS=-mfloat-abi=softfp -march=armv5te -mfpu=vfp -mthumb
#CFLAGS=-mfloat-abi=soft

$(target): $(target).c
	$(CC) $(CFLAGS) -o $@ $< -lm

clean:
	$(RM) $(target)
