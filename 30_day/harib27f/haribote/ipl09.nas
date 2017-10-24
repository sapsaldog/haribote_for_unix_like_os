; haribote-ipl
; TAB=4

CYLS	EQU		9				; 어디까지 읽어들일까

		ORG		0x7c00			; 이 프로그램이 어디에 읽히는 것인가

 이하는 표준적인 FAT12 포맷 플로피 디스크를 위한 기술

		JMP		entry
		DB		0x90
		DB		"HARIBOTE"		; boot sector의 이름을 자유롭게 써도 좋다(8바이트)
		DW		512			; 1섹터 크기(512로 해야 함)
		DB		1			; 클러스터 크기(1섹터로 해야 함)
		DW		1			; FAT가 어디에서 시작될까(보통은 1섹터째부터)
		DB		2			; FAT 개수(2로 해야 함)
		DW		224			; 루트 디렉토리 영역의 크기(보통은 224엔트리로 한다)
		DW		2880			; 드라이브 크기(2880섹터로 해야 함)
		DB		0xf0			; 미디어 타입(0xf0로 해야 함)
		DW		9			; FAT영역의 길이(9섹터로 해야 함)
		DW		18			; 1트럭에 몇개의 섹터가 있을까(18로 해야 함)
		DW		2			; 헤드 수(2로 해야 함)
		DD		0			; 파티션을 사용하지 않기 때문에 여기는 반드시 0
		DD		2880			; 드라이브 크기를 한번 더 write
		DB		0,0,0x29		; 잘 모르지만 이 값으로 해 두면 좋은 것 같다
		DD		0xffffffff		; 아마, 볼륨 시리얼 번호
		DB		"HARIBOTEOS "		; 디스크 이름(11바이트)
		DB		"FAT12   "		; 포맷 이름(8바이트)
		RESB	18				; 우선 18바이트를 비어 둔다

; 프로그램 본체

entry:
		MOV		AX, 0			; 레지스터 초기화
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; 디스크를 읽는다

		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH, 0			; 실린더 0
		MOV		DH, 0			; 헤드 0
		MOV		CL, 2			; 섹터 2
		MOV		BX, 18*2*CYLS-1		; 읽어들이고 싶은 합계 섹터수
		CALL	readfast			; 고속 read

; 다 읽었으므로 haribote.sys를 실행이다!

		MOV		BYTE [0x0ff0], CYLS	; IPL이 어디까지 읽었는지를 메모
		JMP		0xc200

error:
		MOV		AX,0
		MOV		ES,AX
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI, 1			; SI에 1을 더한다
		CMP		AL,0
		JE		fin
		MOV		AH, 0x0e		; 한 글자 표시 펑션
		MOV		BX, 15			; 칼라 코드
		INT		0x10			; 비디오 BIOS 호출
		JMP		putloop
fin:
		HLT					; 무엇인가 있을 때까지 CPU를 정지시킨다
		JMP		fin			; endless loop
msg:
		DB		0x0a, 0x0a		; 개행을 2개
		DB		"load error"
		DB		0x0a			; 개행
		DB		0

readfast:	; AL를 사용해 가능한 한 정리해 읽어낸다
;	ES:read 번지, CH:실린더, DH:헤드, CL:섹터, BX:read 섹터수

		MOV		AX, ES			; < ES로부터 AL의 최대치를 계산 >
		SHL		AX, 3			; AX를 32로 나누어, 그 결과를 AH에 넣은 것이 된다 (SHL는 left shift 명령)
		AND		AH, 0x7f		; AH는 AH를 128로 나눈 나머지(512*128=64 K)
		MOV		AL, 128			; AL = 128 - AH; 제일 가까운 64KB경계까지 들어갈 최대 섹터
		SUB		AL,AH

		MOV		AH, BL			; < BX로부터 AL의 최대치를 AH에 계산 >
		CMP		BH, 0			; if (BH ! = 0) { AH = 18; }
		JE		.skip1
		MOV		AH,18
.skip1:
		CMP		AL, AH			; if (AL > AH) { AL = AH; }
		JBE		.skip2
		MOV		AL,AH
.skip2:

		MOV		AH, 19			; < CL로부터 AL의 최대치를 AH에 계산 >
		SUB		AH, CL			; AH = 19 - CL;
		CMP		AL, AH			; if (AL > AH) { AL = AH; }
		JBE		.skip3
		MOV		AL,AH
.skip3:

		PUSH	BX
		MOV		SI, 0			; 실패 회수를 세는 레지스터
retry:
		MOV		AH, 0x02		; AH=0x02 : 디스크 read
		MOV		BX,0
		MOV		DL, 0x00		; A드라이브
		PUSH	ES
		PUSH	DX
		PUSH	CX
		PUSH	AX
		INT		0x13			; 디스크 BIOS 호출
		JNC		next			; 에러가 일어나지 않으면 next에
		ADD		SI, 1			; SI에 1을 더한다
		CMP		SI, 5			; SI와 5를 비교
		JAE		error			; SI >= 5 라면 error에
		MOV		AH,0x00
		MOV		DL, 0x00		; A드라이브
		INT		0x13			; 드라이브의 리셋트
		POP		AX
		POP		CX
		POP		DX
		POP		ES
		JMP		retry
next:
		POP		AX
		POP		CX
		POP		DX
		POP		BX			; ES의 내용을 BX로 받는다
		SHR		BX, 5			; BX를 16바이트 단위부터 512바이트 단위에
		MOV		AH,0
		ADD		BX, AX			; BX += AL;
		SHL		BX, 5			; BX를 512바이트 단위부터 16바이트 단위에
		MOV		ES, BX			; 이것으로 ES += AL * 0x20; 가 된다
		POP		BX
		SUB		BX,AX
		JZ		.ret
		ADD		CL, AL			; CL에 AL를 더한다
		CMP		CL, 18			; CL와 18을 비교
		JBE		readfast		; CL <= 18 이라면 readfast에
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readfast		; DH < 2 라면 readfast에
		MOV		DH,0
		ADD		CH,1
		JMP		readfast
.ret:
		RET

		RESB	0x7dfe-$			; 0x7dfe까지를 0x00로 채우는 명령

		DB		0x55, 0xaa
