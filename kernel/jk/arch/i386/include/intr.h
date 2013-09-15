#ifndef __INTR_H
#define __INTR_H


/* x86 exceptions (0 - 31) */
#define INTR_DIVERR		0
#define INTR_DEBUG		1
#define INTR_NMI		2
#define INTR_BRKPNT		3
#define INTR_OVRFLW		4
#define INTR_BOUNDS		5
#define INTR_INVOP		6
#define INTR_NOCOPR		7
#define INTR_DOUBLE		8
#define INTR_INVTSS		10
#define INTR_NOSEG		11
#define INTR_STACK		12
#define INTR_GENPROT		13
#define INTR_PGFLT		14
#define INTR_COPRERR		16

/* PC ISA interrupts (32 - 47) */
#define INTR_TMR		32
#define INTR_KBD		33
#define INTR_SERALT		35
#define INTR_SERPRI		36
#define INTR_FD			38
#define INTR_PARA		39
#define INTR_RTC		40
#define INTR_AUX		44
#define INTR_MATHCOPRERR	45
#define INTR_HD			46

/* Kernel interrupts (48 - 63) */
#define INTR_SYSCALL		48


typedef void    (*isr_func_t) ();

typedef struct isr {
    isr_func_t      f;
    struct isr     *next;
}              *isr_t;


int             intr_mask(int intr);
int             intr_unmask(int intr);
int             isr_install(int intr, isr_func_t f);
int             isr_uninstall(int intr, isr_func_t f);


#endif
