#ifndef __SELECTOR_H
#define __SELECTOR_H


#define SEL_SIZE	8	       /* Selector size */

#define GDT_SIZE	5	       /* Non-tss descriptors in gdt */

#define NULLSEL		0	       /* Null */
#define KCSEL		0x08	       /* Kernel code */
#define KDSEL		0x10	       /* Kernel data */
#define UCSEL		0x18	       /* User code */
#define UDSEL		0x20	       /* User data */


#endif
