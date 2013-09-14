CFLAGS=-g3

all:injector trace-cnt reverse dummy-loop-prog dummy1 singlestep attach breakpoint watchpoint pt

dummy-loop-prog: dummy-loop-prog.c
	$(CC) $(CFLAGS) -o $@ $<

injector: injector.c
	$(CC) $(CFLAGS) -o $@ $<

trace-cnt: trace-cnt.c
	$(CC) $(CFLAGS) -o $@ $<

singlestep: singlestep.c
	$(CC) $(CFLAGS) -o $@ $<

dummy1: dummy1.s
	$(CC) $(CFLAGS) -o $@ $<

reverse: reverse.c
	$(CC) $(CFLAGS) -o $@ $<

attach: attach.c
	$(CC) $(CFLAGS) -o $@ $<

breakpoint: breakpoint.c
	$(CC) $(CFLAGS) -o $@ $<

watchpoint: watchpoint.c
	$(CC) $(CFLAGS) -o $@ $<

pt: pt.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) trace-cnt reverse injector dummy-loop-prog dummy1 singlestep attach breakpoint watchpoint pt
