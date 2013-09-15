/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 *
 * Contributor: JImchyun Huang <jimchyun@ccns.ncku.edu.tw>
  */

#include "internal.h"

struct interrupt_handler {
	kuint (*handler)(kuint number);
};

static struct interrupt_handler interrupt_handler[49];

static kuint cached_irq_mask;
#define cached_21 (((kuint8*)(&(cached_irq_mask)))[0])
#define cached_A1 (((kuint8*)(&(cached_irq_mask)))[1])

volatile kuint kirq_spinlock;

kfunction void kirq_init(void) {
	extern kuint16 idt[];
	kuint i = 0;

	for(i = 0; i < 49; i++) {
		idt[(i * 4) + 1] = (kuint16)(0x0010);
		idt[(i * 4) + 2] = (kuint16)(0x8E00);
	}

	/*
	 * Initialize the Programmable Interrupt Controller (PIC)
	 */

	cached_irq_mask = 0xFFFB; /* IRQ2 is enabled */

	kioport_out_kuint8(0x20, 0x10 + 0x01); /* ICW1: ICW Select, ICW4 write required */
	kioport_out_kuint8(0xA0, 0x10 + 0x01);
	kioport_out_kuint8(0x21, 0x20); /* ICW2: Interrupt vector address */
	kioport_out_kuint8(0xA1, 0x28);
	kioport_out_kuint8(0x21, 0x04); /* ICW3: Uses interrupt 2 for communication between PIC1 and PIC 2*/
	kioport_out_kuint8(0xA1, 0x02);
	kioport_out_kuint8(0x21, 0x01); /* ICW4: 286/386 microprocessor mode */
	kioport_out_kuint8(0xA1, 0x01);
	kioport_out_kuint8(0x21, cached_21); /* OCW1: Interrupt mask register */
	kioport_out_kuint8(0xA1, cached_A1);
}

void handle_exception(kuint number, struct processor_regs* regs) 
{
	kuint cr0 = 0;
	kuint cr1 = 0;
	kuint cr2 = 0;
	kuint cr3 = 0;
	kuint cr4 = 0;

	asm volatile(
			"movl %%cr0, %0 ;"
			"/*movl %%cr1, %1 ;*/"
			"movl %%cr2, %2 ;"
			"movl %%cr3, %3 ;"
			"/*movl %%cr4, %4 ;*/"
			: "=r" (cr0), "=r" (cr1), "=r" (cr2), "=r" (cr3), "=r" (cr4)
			: "0" (cr0), "1" (cr1), "2" (cr2), "3" (cr3), "4" (cr4)
		    );

	kprintf("Exception=%02x\n", number);
	kprintf("eflags=%0.8x cs=%0.8x eip=%0.8x error=%0.8x\n", regs->eflags, regs->cs, regs->eip, regs->error);
	kprintf("eax=%0.8x ebx=%0.8x ecx=%0.8x edx=%0.8x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
	kprintf("edi=%0.8x esi=%0.8x ebp=%0.8x esp=%0.8x\n", regs->edi, regs->esi, regs->ebp, regs->esp);
	kprintf("cr0=%0.8x cr1=%0.8x cr2=%0.8x cr3=%0.8x cr4=%0.8x\n", cr0, cr1, cr2, cr3, cr4);

	print_stack_trace(regs);

	kthread_kill_current();
}

void handle_irq(kuint number, struct processor_regs* regs) 
{
	kuint irq = number - 32;
	kuint rc = 0;

	kspinlock_lock(&kirq_spinlock);

	cached_irq_mask |= (1 << irq);

	if(irq & 8) {
		kioport_out_kuint8(0xA1, cached_A1); /* OCW1: Mask interrupt */
		kioport_out_kuint8(0xA0, 0x60 + (irq & 7)); /* OCW2: Specific EOI, IRQ# */
		kioport_out_kuint8(0x20, 0x60 + 0x02); /* OCW2: Specific EOI, IRQ2 */
	} else {
		kioport_out_kuint8(0x21, cached_21); /* OCW1: Mask interrupt */
		kioport_out_kuint8(0x20, 0x60 + irq); /* OCW2: Specific EOI, IRQ# */
	}

	if(interrupt_handler[number].handler) {
		rc = interrupt_handler[number].handler(irq);
	} else {
		kprintf("IRQ %x\n", irq);
		handle_exception(number, regs);
	}

	kspinlock_unlock(&kirq_spinlock);

	kirq_enable_irq(irq);

	if(rc) {
		kthread_yield();
	}
}

kfunction void kirq_enable_irq_internal(kuint irq) {
	cached_irq_mask &= (~(1 << irq));

	if(irq & 8) {
		kioport_out_kuint8(0xA1, cached_A1); /* OCW1: Mask interrupt */
	} else {
		kioport_out_kuint8(0x21, cached_21); /* OCW1: Mask interrupt */
	}
}

kfunction void kirq_enable_irq(kuint irq) {
	kspinlock_lock(&kirq_spinlock);
	kirq_enable_irq_internal(irq);
	kspinlock_unlock(&kirq_spinlock);
}

kfunction void kirq_disable_irq_internal(kuint irq) {
	cached_irq_mask |= (1 << irq);

	if(irq & 8) {
		kioport_out_kuint8(0xA1, cached_A1); /* OCW1: Mask interrupt */
	} else {
		kioport_out_kuint8(0x21, cached_21); /* OCW1: Mask interrupt */
	}
}

kfunction void kirq_disable_irq(kuint irq) {
	kspinlock_lock(&kirq_spinlock);
	kirq_disable_irq_internal(irq);
	kspinlock_unlock(&kirq_spinlock);
}

kfunction void kirq_enable_all(void) {
	asm volatile(
		"sti ;"
	);
}

kfunction void kirq_disable_all(void) {
	asm volatile(
		"cli ;"
	);
}

kfunction kuint kirq_assign_irq(kuint irq, kuint (*handler)(kuint irq)) {
	kuint rc = 0;

	kspinlock_lock(&kirq_spinlock);

	if(irq < 16) {
		if(interrupt_handler[irq + 32].handler == KNULL) {
			interrupt_handler[irq + 32].handler = handler;
			kirq_enable_irq_internal(irq);

			rc = 1;
		}
	}

	kspinlock_unlock(&kirq_spinlock);

	return rc;
}

kfunction void kirq_unassign_irq(kuint irq) {
	kspinlock_lock(&kirq_spinlock);

	if(irq < 16) {
		kirq_disable_irq_internal(irq);
		interrupt_handler[irq + 32].handler = KNULL;
	}

	kspinlock_unlock(&kirq_spinlock);
}

