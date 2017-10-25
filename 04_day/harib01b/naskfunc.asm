; naskfunc
; TAB=4

[bits 32]				; 486��ɱ��� ����ϰ� �ʹٰ� �ϴ� ���

global io_hlt
global write_mem8
global _start;

_start:
    [extern HariMain] ; Define calling point. Must have same name as kernel.c 'main' function
    call HariMain ; Calls the C function. The linker will know where it is placed in memory

io_hlt:	; void io_hlt(void);
		HLT
		RET

write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP+4]�� addr�� �� �����Ƿ� �װ��� ECX�� read
		MOV		AL,[ESP+8]		; [ESP+8]�� data�� �� �����Ƿ� �װ��� AL�� read
		MOV		[ECX],AL
		RET
