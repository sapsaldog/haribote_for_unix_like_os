/* 키보드 관계 */

#include "bootpack.h"

struct FIFO32 *keyfifo;
int keydata0;

void inthandler21(int *esp)
{
	int data;
	io_out8(PIC0_OCW2, 0x61);	/* IRQ-01 접수 완료를 PIC에 통지 */
	data = io_in8(PORT_KEYDAT);
	fifo32_put(keyfifo, data + keydata0);
	return;
}

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void)
{
	/* 키보드 컨트롤러가 데이터 송신이 가능하게 되는 것을 기다린다 */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(struct FIFO32 *fifo, int data0)
{
	/* write할 FIFO 버퍼를 기억 */
	keyfifo = fifo;
	keydata0 = data0;
	/* 키보드컨트롤러의 초기화 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}
