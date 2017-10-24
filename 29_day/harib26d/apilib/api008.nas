[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api008.nas"]

		GLOBAL	_api_initmalloc

[SECTION .text]

_api_initmalloc:	; void api_initmalloc(void);
		PUSH	EBX
		MOV		EDX,8
		MOV		EBX,[CS:0x0020]		; malloc 영역의 번지
		MOV		EAX,EBX
		ADD		EAX,32*1024		; 32KB를 더한다
		MOV		ECX,[CS:0x0000]		; 데이터 세그먼트의 크기
		SUB		ECX,EAX
		INT		0x40
		POP		EBX
		RET
