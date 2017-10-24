void io_hlt(void);
void write_mem8(int addr, int data);

void HariMain(void)
{
	int i; /* 변수 선언.i라고 하는 변수는 32 비트의 정수형 */

	for (i = 0xa0000; i <= 0xaffff; i++) {
		write_mem8(i, 15); /* MOV BYTE [i], 15 */
	}

	for (;;) {
		io_hlt();
	}
}
