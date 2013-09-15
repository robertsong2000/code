/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

	.code32

.extern	setup

	.balign	0x10
.global	_start
_start:
	cli
	cmpl	$0x2BADB002, %eax /* Did we get loaded by GRUB?  */
0:	jne	0b
	pushl	%ebx
	call	setup
	lgdt	gdtr
	lldt	ldtr
	lidt	idtr
	movl	virtual_page_table_directory, %ebx
	movl	%ebx, %cr3
	movl	%cr0, %ebx
	orl	$0x80000003, %ebx /* Paging (31), Monitor Coprocessor (1), Protection Enable (0).  */
	movl	%ebx, %cr0
	ljmp	$0x10, $.Lpm /* Jump to protected mode.  */

	.balign	0x08
.Lpm:
	movl	$0x18, %ebx
	movw	%bx, %ds
	movw	%bx, %es
	movw	%bx, %fs
	movw	%bx, %gs
	movw	%bx, %ss
	movl	%eax, %esp
	pushl	$0x00000000
	pushl	$kthread_kill_current
	jmp	init

	.balign	0x08
	/* GRUP Multiboot Header.  */
multiboot_header:
	.long	0x1BADB002 /* magic */
	.long	0x00000003 /* flags */
	.long	(0x00000000 - (0x00000003 + 0x1BADB002)) /* checksum */

	.data

	.balign	0x10
.global	gdt
gdt:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00 /* 0x10 */
	.byte	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00 /* 0x18 */
	.byte	0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFA, 0xCF, 0x00 /* 0x20 */
	.byte	0xFF, 0xFF, 0x00, 0x00, 0x00, 0xF2, 0xCF, 0x00 /* 0x28 */

	.balign 0x08
	.word	0x0000
idtr:
	.word	((49 * 8) - 1)
	.long	idt

	.balign 0x08
	.word	0x0000
gdtr:
	.word	((6 * 8) - 1)
	.long	gdt

	.balign 0x08
	.word	0x0000
ldtr:
	.word	0x00000000
	.long	0x00000000

