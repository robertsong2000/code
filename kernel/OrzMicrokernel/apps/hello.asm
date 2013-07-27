%include "macro.inc"
%include "orzmicro.inc"

		db "ORZEXE.0"		; Executable file revision 0 id
		dd 0x1000		; Stack size
		dd 0			; Data size

[ORG -0x10]
[BITS 32]

	xor ebx, ebx
	mov eax, msg_CreateTask
	mov esi, msg_task
	mov ecx, task_size
	xor edx, edx
	int i_SendMessageW

	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	mov eax, msg_Exit
	int i_SendMessageW

%include "console_library.inc"
%include "debug.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

msg_task	db "hello_"
		db 0
		times (MAX_TASK_NAME + 1) - ($-msg_task) db 0

		incbin "bin/hello_impl.bin"
task_size	equ $-msg_task
