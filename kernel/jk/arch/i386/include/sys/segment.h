#ifndef __SEGMENT_H
#define __SEGMENT_H


#include <sys/types.h>


struct pseudodesc {
    u_long          eip;
    u_short         cs;
}               __attribute__((packed));


struct tss {
    u_long          back;
    u_long          esp0;
    u_long          ss0;
    u_long          esp1;
    u_long          ss1;
    u_long          esp2;
    u_long          ss2;
    u_long          cr3;
    u_long          eip;
    u_long          eflags;
    u_long          eax;
    u_long          ecx;
    u_long          edx;
    u_long          ebx;
    u_long          esp;
    u_long          ebp;
    u_long          esi;
    u_long          edi;
    u_long          es;
    u_long          cs;
    u_long          ss;
    u_long          ds;
    u_long          fs;
    u_long          gs;
    u_long          ldt;
    u_long          ioperm;
}               __attribute__((packed));


struct tssdesc {
    u_short         limit0;
    u_short         base0;
    u_char          base16;
    u_char          type;
    u_char          limit16;
    u_char          base24;
}               __attribute__((packed));


typedef struct tss *tss_t;
typedef struct tssdesc *tssdesc_t;


void            ltr(u_long selector);
void            jmptss(void *tssdesc);

void
tssinit(tss_t tss,
	u_long kstk,
	size_t kstksize,
	u_long stk,
	size_t stksize);


#endif
