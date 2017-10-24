[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "crack7.nas"]

		GLOBAL	_HariMain

[SECTION .text]

_HariMain:
		MOV		AX,1005*8
		MOV		DS,AX
		CMP		DWORD [DS:0x0004],'Hari'
		JNE		fin			; 어플리케이션은 아닌 것 같아서 아무것도 하지 않는다

		MOV		ECX,[DS:0x0000]		; 이 어플리케이션의 데이터 세그먼트 크기를 읽어낸다
		MOV		AX,2005*8
		MOV		DS,AX

crackloop:						; 123으로 다 채운다
		ADD		ECX,-1
		MOV		BYTE [DS:ECX],123
		CMP		ECX,0
		JNE		crackloop

fin:							; 종료
		MOV		EDX,4
		INT		0x40
