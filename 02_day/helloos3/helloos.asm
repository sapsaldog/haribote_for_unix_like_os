; hello-os
; TAB=4

[org 0x7c00] ; 이 프로그램이 어디에 read되는가

; 이하는 표준적인 FAT12 포맷 플로피 디스크를 위한 기술

JMP		entry

; 프로그램 본체

entry:
mov		AX, 0			; 레지스터 초기화
mov		SS,AX
mov		SP,0x7c00
mov		DS,AX
mov		ES,AX

mov		SI,msg
putloop:
mov		AL,[SI]
add		SI, 1			; SI에 1을 더한다
CMP		AL,0
JE		fin
mov		AH, 0x0e		; 한 글자 표시 Function
mov		BX, 15			; 칼라 코드
INT		0x10			; 비디오 BIOS 호출
JMP		putloop
fin:
HLT					; 무엇인가 있을 때까지 CPU를 정지시킨다
JMP		fin			; Endless Loop

msg:
db		0x0a, 0x0a		; 개행을 2개
db		"hello, world"
db		0x0a			; 개행
db		0

times 510 - ($-$$) db 0
db		0x55, 0xaa

; 이하는 boot sector이외의 부분을 기술

db		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
resb	4600
db		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
resb	1469432
