gp_except_tx	db "Exception "
gp_except_txN	db ".."
		db ", "
gp_except_txS	db "...."
		db ":"
gp_except_txO	db "........"
		db " code "
gp_except_txC	db "........"
		db 0

except_bk	dd 0x12345678

Exception:
	push eax
	mov ax, kernel_data - _GDT
	mov ds, ax
	mov es, ax

	pop eax
	mov [except_bk], eax
	pop ax		; exception number

	pushad
	push es
	push fs
	push gs
	mov edi, gp_except_txN
	call PrintHexByte
	pop gs
	pop fs
	pop es
	popad

	pushad
	push es
	push fs
	push gs
	mov edi, gp_except_txC
	mov al, '#'
	mov ecx, 8
	rep stosb
	pop gs
	pop fs
	pop es
	popad

	pop eax	; Offset
	pushad
	push es
	push fs
	push gs
	mov edi, gp_except_txO
	call PrintHexDword
	pop gs
	pop fs
	pop es
	popad

	pop ax	; Selector
	pushad
	push es
	push fs
	push gs
	mov edi, gp_except_txS
	call PrintHexWord
	pop gs
	pop fs
	pop es
	popad

	mov [gp_print_tx], DWORD gp_except_tx
	mov eax, [except_bk]
	jmp GP_FAULT

;;;;

ExceptionC:
	push eax
	mov ax, kernel_data - _GDT
	mov ds, ax
	mov es, ax

	pop eax
	mov [except_bk],eax
	pop ax		; exception number

	pushad
	push es
	push fs
	push gs
	mov edi, gp_except_txN
	call PrintHexByte
	pop gs
	pop fs
	pop es
	popad

	pop eax		; Exception code
	pushad
	push es
	push fs
	push gs
	mov edi, gp_except_txC
	call PrintHexDword
	pop gs
	pop fs
	pop es
	popad

	pop eax		; Offset
	pushad
	push es
	push fs
	push gs
	mov edi, gp_except_txO
	call PrintHexDword
	pop gs
	pop fs
	pop es
	popad

	pop ax		; Selector
	pushad
	push es
	push fs
	push gs
	mov edi, gp_except_txS
	call PrintHexWord
	pop gs
	pop fs
	pop es
	popad

	mov [gp_print_tx], DWORD gp_except_tx
	mov eax, [except_bk]
	jmp GP_FAULT

;-------------------------------------------------------------------------------

%macro except_body 2
	cli
	push WORD %1
	jmp %2
%endmacro

Except_00:
	except_body 0x00, Exception

Except_01:
	except_body 0x01, Exception

Except_02:
	except_body 0x02, Exception

Except_03:
	except_body 0x03, Exception

Except_04:
	except_body 0x04, Exception

Except_05:
	except_body 0x05, Exception

Except_06:
	except_body 0x06, Exception

Except_07:
	except_body 0x07, Exception

Except_08:
	except_body 0x08, ExceptionC

Except_09:
	except_body 0x09, Exception

Except_0A:
	except_body 0x0A, ExceptionC

Except_0B:
	except_body 0x0B, ExceptionC

Except_0C:
	except_body 0x0C, ExceptionC

Except_0D:
	except_body 0x0D, ExceptionC

Except_0E:
	except_body 0x0E, Exception

Except_0F:
	except_body 0x0F, Exception

Except_10:
	except_body 0x10, Exception

Except_11:
	except_body 0x11, Exception

Except_12:
	except_body 0x12, Exception

Except_13:
	except_body 0x13, Exception

Except_14:
	except_body 0x14, Exception

Except_15:
	except_body 0x15, Exception

Except_16:
	except_body 0x16, Exception

Except_17:
	except_body 0x17, Exception

Except_18:
	except_body 0x18, Exception

Except_19:
	except_body 0x19, Exception

Except_1A:
	except_body 0x1A, Exception

Except_1B:
	except_body 0x1B, Exception

Except_1C:
	except_body 0x1C, Exception

Except_1D:
	except_body 0x1D, Exception

Except_1E:
	except_body 0x1E, Exception

Except_1F:
	except_body 0x1F, Exception
