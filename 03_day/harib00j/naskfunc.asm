; iolib
; TAB=4

[bits 32]					; 32비트 모드용의 기계어를 만든다

GLOBAL	io_hlt

_start:
    [extern HariMain] ; Define calling point. Must have same name as kernel.c 'main' function
    call HariMain ; Calls the C function. The linker will know where it is placed in memory

io_hlt:	; void io_hlt(void);
		HLT
		RET
