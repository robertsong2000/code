	.cpu arm926ej-s
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"fpic.c"
	.text
	.align	2
	.global	call_bar
	.type	call_bar, %function
call_bar:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	ldr	r3, .L3
.LPIC0:
	add	r3, pc, r3
	ldr	r2, .L3+4
	ldr	r3, [r3, r2]
	ldr	r3, [r3, #0]
	mov	r0, r3
	bl	bar(PLT)
	mov	r3, r0
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L4:
	.align	2
.L3:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC0+8)
	.word	foo(GOT)
	.size	call_bar, .-call_bar
	.ident	"GCC: (20100621 (Sony CE Linux 6.0.0.4)) 4.3.3"
	.section	.note.GNU-stack,"",%progbits
