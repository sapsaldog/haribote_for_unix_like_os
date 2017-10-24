; haribote-os
; TAB=4
[bits 16]
[org 0x8200]

; BOOT_INFO관계
		CYLS	EQU		0x0ff0			; boot sector가 설정한다
		LEDS	EQU		0x0ff1
		VMODE	EQU		0x0ff2			; 색 가지수에 관한 정보.어떤 비트 칼라인가?
		SCRNX	EQU		0x0ff4			; 해상도의 X
		SCRNY	EQU		0x0ff6			; 해상도의 Y
		VRAM	EQU		0x0ff8			; 그래픽 버퍼의 개시 번지

		MOV		AL, 0x13	; VGA 그래픽스, 320 x200x8bit 칼라
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE], 8	; 화면 모드를 write한다
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; 키보드의 LED상태를 BIOS가 알려준다
		MOV		AH,0x02
		INT		0x16 		; keyboard BIOS
		MOV		[LEDS],AL

fin:
		HLT
		JMP		fin
