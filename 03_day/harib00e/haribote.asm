; haribote-os
; TAB=4
[bits 16]
[org 0x8200]

fin:
		HLT
		JMP		fin
