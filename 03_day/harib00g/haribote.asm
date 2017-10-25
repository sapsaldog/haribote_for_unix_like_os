; haribote-os
; TAB=4
[bits 16]
[org 0x8200]

		MOV		AL, 0x13		; VGA 그래픽스, 320 x200x8bit 칼라
		MOV		AH,0x00
		INT		0x10
fin:
		HLT
		JMP		fin
