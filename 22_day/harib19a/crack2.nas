[INSTRSET "i486p"]
[BITS 32]
		MOV		EAX,1*8			; OS용의 세그먼트(segment) 번호
		MOV		DS, AX			; 이것을 DS에 넣어버린다
		MOV		BYTE [0x102600],0
		MOV		EDX,4
		INT		0x40
