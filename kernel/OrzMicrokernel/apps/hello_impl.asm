%include "macro.inc"
%include "orzmicro.inc"

		dd 0x1000		; Stack size
		dd _DATA_LEN		; Data size

[ORG -8]
[BITS 32]

	mov esi, strx
	mov ecx, strx_len
	call __dbg_PrintStrS

	;;;;

Exit:
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	mov eax, msg_Exit
	int i_SendMessageW

%include "console_library.inc"
%include "debug.inc"

;;;;

strx		db "Hello World!", 0x0D
strx_len	equ $-strx

;---------------------------------------------------------------
;;;; DATA
;---------------------------------------------------------------
_DATA_START	equ $

tb		equ _DATA_START
tb_len		equ 20000
_end_tb		equ tb + tb_len

_DATA_LEN	equ _end_tb - _DATA_START
