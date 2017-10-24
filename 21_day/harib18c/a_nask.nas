[FORMAT "WCOFF"]				; 오브젝트 파일을 만드는 모드	
[INSTRSET "i486p"]				; 486명령까지 사용하고 싶다고 하는 기술
[BITS 32]					; 32비트 모드용의 기계어를 만들게 한다
[FILE "a_nask.nas"]				; 원시 파일명 정보

		GLOBAL	_api_putchar

[SECTION .text]

_api_putchar:	; void api_putchar(int c);
		MOV		EDX,1
		MOV		AL,[ESP+4]	; c
		INT		0x40
		RET
