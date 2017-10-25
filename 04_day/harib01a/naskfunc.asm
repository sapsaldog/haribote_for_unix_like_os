; iolib
; TAB=4

[bits 32]					; 32비트 모드용의 기계어를 만든다

GLOBAL	io_hlt, write_mem8

_start:
    [extern HariMain] ; Define calling point. Must have same name as kernel.c 'main' function
    call HariMain ; Calls the C function. The linker will know where it is placed in memory

io_hlt:	; void io_hlt(void);
		HLT
		RET

write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP+4]에 addr가 들어가 있으므로 그것을 ECX에 read한다
		MOV		AL,[ESP+8]		; [ESP+8]에 data가 들어가 있으므로 그것을 AL에 read한다
		MOV		[ECX],AL
		RET
