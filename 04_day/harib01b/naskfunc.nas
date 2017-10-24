; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 오브젝트 파일을 만드는 모드	
[INSTRSET "i486p"]				; 486의 명령까지 사용하고 싶다고 하는 기술
[BITS 32]					; 32 비트 모드용의 기계어를 만든다
[FILE "naskfunc.nas"]				; 원시 파일명 정보

		GLOBAL	_io_hlt,_write_mem8

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP+4]에 addr가 들어가 있으므로 그것을 ECX에 read
		MOV		AL,[ESP+8]		; [ESP+8]에 data가 들어가 있으므로 그것을 AL에 read
		MOV		[ECX],AL
		RET
