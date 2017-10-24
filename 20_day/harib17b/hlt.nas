[BITS 32]
		MOV		AL,'A'
		CALL    0xbcc
fin:
		HLT
		JMP		fin
