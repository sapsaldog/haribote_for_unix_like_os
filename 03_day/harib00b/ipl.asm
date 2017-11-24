; haribote-ipl
; TAB=4

[org 0x7c00] ; 이 프로그램이 어디에 read되는가

; 이하는 표준적인 FAT12 포맷 플로피 디스크를 위한 기술

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
		DW		9			; FAT영역 길이(9섹터로 해야 함)
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

		MOV		SI, 0			; 실패 회수를 세는 레지스터
retry:
		MOV		AH, 0x02		; AH=0x02 : 디스크 read
		MOV		AL, 1			; 1섹터
		MOV		BX,0
		MOV		DL, 0x00		; A드라이브
		INT		0x13			; 디스크 BIOS 호출
		JNC		fin			; 에러가 일어나지 않으면 fin에
		ADD		SI, 1			; SI에 1을 더한다
		CMP		SI, 5			; SI와 5를 비교
		JAE		error			; SI >= 5이면 error에
		MOV		AH,0x00
		MOV		DL, 0x00		; A드라이브
		INT		0x13			; 드라이브의 리셋트
		JMP		retry

; 다 읽었지만 우선 할일이 없기 때문에 sleeve

fin:
		HLT					; 무엇인가 있을 때까지 CPU를 정지시킨다
		JMP		fin			; Endless Loop

error:
		MOV		AX,0
		MOV		ES,AX
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI, 1			; SI에 1을 더한다
		CMP		AL,0
		JE		fin
		MOV		AH, 0x0e		; 한 글자 표시 function
		MOV		BX, 15			; 칼라 코드
		INT		0x10			; 비디오 BIOS 호출
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; 개행을 2개
		DB		"load error"
		DB		0x0a			; 개행
		DB		0

		times 510 - ($-$$) db 0 ; 0x7dfe까지를 0x00로 채우는 명령

		DB		0x55, 0xaa
