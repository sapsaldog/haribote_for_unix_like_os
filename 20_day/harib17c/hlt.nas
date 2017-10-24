[BITS 32]
		MOV		AL,'A'
		CALL    2*8:0xbcc
fin:
		HLT
		JMP		fin
