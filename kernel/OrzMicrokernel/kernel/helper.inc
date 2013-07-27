%macro GDT_Adjustment 0
push eax
mov ax, kernel_data - _GDT
mov ds, ax
mov es, ax
mov gs, ax
mov fs, ax                                                                                          
pop eax
%endmacro
