; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 오브젝트 파일을 만드는 모드	
[BITS 32]					; 32 비트 모드용의 기계어를 만든다


; 오브젝트 파일을 위한 정보

[FILE "naskfunc.nas"]				; 원시 파일명 정보

		GLOBAL	_io_hlt			; 이 프로그램에 포함되는 함수명


; 이하는 실제의 함수

[SECTION .text]				; 오브젝트 파일에서는 이것을 쓰고 나서 프로그램을 쓴다

_io_hlt:	; void io_hlt(void);
		HLT
		RET
