/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

	.text

	.macro	SAVEREGS
	/* save current thread */
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%edi
	pushl	%esi
	pushl	%ebp
	pushl	%esp
	.endm

	.macro	RESTOREREGS
	/* restore next thread */
	popl	%esp
	popl	%ebp
	popl	%esi
	popl	%edi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	.endm

	.macro	ISR int_num error_code isr_handler
	.balign	0x10
.Lisr_\int_num:
	.if	!\error_code
	pushf
	.endif
	SAVEREGS
	pushl	%esp
	.byte	0x6A, \int_num
	call	\isr_handler
	addl	$8, %esp
	RESTOREREGS
	addl	$4, %esp
	iret
	.endm

	.balign	0x10
.global	isr
isr:
	ISR	0x00 0 handle_exception /* Divide Error */
	ISR	0x01 0 handle_exception /* Debug Exceptions */
	ISR	0x02 0 handle_exception /* NMI Interrupt */
	ISR	0x03 0 handle_exception /* Breakpoint */
	ISR	0x04 0 handle_exception /* Overflow */
	ISR	0x05 0 handle_exception /* BOUND Range Exceded */
	ISR	0x06 0 handle_exception /* Invalid Opcode */
	ISR	0x07 0 handle_exception /* Device Not Available (FPU) */
	ISR	0x08 1 handle_exception /* Double Fault */
	ISR	0x09 0 handle_exception /* Coprocessor Segment Overrun */
	ISR	0x0A 1 handle_exception /* Invalid TSS */
	ISR	0x0B 1 handle_exception /* Segment Not Present */
	ISR	0x0C 1 handle_exception /* Stack-Segment Fault */
	ISR	0x0D 1 handle_exception /* General Protection */
	ISR	0x0E 1 handle_page_fault /* Page Fault */
	ISR	0x0F 0 handle_exception /* */
	ISR	0x10 0 handle_exception /* Floating-Point Error */
	ISR	0x11 1 handle_exception /* Alignment Chack */
	ISR	0x12 0 handle_exception /* Machine Check */
	ISR	0x13 0 handle_exception /* Streaming SIMD Extensions */
	ISR	0x14 0 handle_exception /* */
	ISR	0x15 0 handle_exception /* */
	ISR	0x16 0 handle_exception /* */
	ISR	0x17 0 handle_exception /* */
	ISR	0x18 0 handle_exception /* */
	ISR	0x19 0 handle_exception /* */
	ISR	0x1A 0 handle_exception /* */
	ISR	0x1B 0 handle_exception /* */
	ISR	0x1C 0 handle_exception /* */
	ISR	0x1D 0 handle_exception /* */
	ISR	0x1E 0 handle_exception /* */
	ISR	0x1F 0 handle_exception /* */
	ISR	0x20 0 handle_irq /* IRQ 0x00 */
	ISR	0x21 0 handle_irq /* IRQ 0x01 */
	ISR	0x22 0 handle_irq /* IRQ 0x02 */
	ISR	0x23 0 handle_irq /* IRQ 0x03 */
	ISR	0x24 0 handle_irq /* IRQ 0x04 */
	ISR	0x25 0 handle_irq /* IRQ 0x05 */
	ISR	0x26 0 handle_irq /* IRQ 0x06 */
	ISR	0x27 0 handle_irq /* IRQ 0x07 */
	ISR	0x28 0 handle_irq /* IRQ 0x08 */
	ISR	0x29 0 handle_irq /* IRQ 0x09 */
	ISR	0x2A 0 handle_irq /* IRQ 0x0A */
	ISR	0x2B 0 handle_irq /* IRQ 0x0B */
	ISR	0x2C 0 handle_irq /* IRQ 0x0C */
	ISR	0x2D 0 handle_irq /* IRQ 0x0D */
	ISR	0x2E 0 handle_irq /* IRQ 0x0E */
	ISR	0x2F 0 handle_irq /* IRQ 0x0F */
	.balign	0x10
.global handle_thread_switch
handle_thread_switch:
	pushfl
	SAVEREGS
	/* switch to the next thread */
	pushl	%esp
	call	thread_switch
	addl	$4, %esp
	RESTOREREGS
	popfl
	iret

	.data

	.macro	IDT int_num
	.long	.Lisr_\int_num, .Lisr_\int_num
	.endm

	.balign	0x10
.global	idt
idt:
	IDT	0x00
	IDT	0x01
	IDT	0x02
	IDT	0x03
	IDT	0x04
	IDT	0x05
	IDT	0x06
	IDT	0x07
	IDT	0x08
	IDT	0x09
	IDT	0x0A
	IDT	0x0B
	IDT	0x0C
	IDT	0x0D
	IDT	0x0E
	IDT	0x0F
	IDT	0x10
	IDT	0x11
	IDT	0x12
	IDT	0x13
	IDT	0x14
	IDT	0x15
	IDT	0x16
	IDT	0x17
	IDT	0x18
	IDT	0x19
	IDT	0x1A
	IDT	0x1B
	IDT	0x1C
	IDT	0x1D
	IDT	0x1E
	IDT	0x1F
	IDT	0x20
	IDT	0x21
	IDT	0x22
	IDT	0x23
	IDT	0x24
	IDT	0x25
	IDT	0x26
	IDT	0x27
	IDT	0x28
	IDT	0x29
	IDT	0x2A
	IDT	0x2B
	IDT	0x2C
	IDT	0x2D
	IDT	0x2E
	IDT	0x2F
	.long	handle_thread_switch, handle_thread_switch

