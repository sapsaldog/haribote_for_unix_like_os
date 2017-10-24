/* 마우스 관계 */

#include "bootpack.h"

struct FIFO32 *mousefifo;
int mousedata0;

void inthandler2c(int *esp)
/* PS/2 마우스로부터의 인터럽트 */
{
	int data;
	io_out8(PIC1_OCW2, 0x64);	/* IRQ-12 접수 완료를 PIC1에 통지 */
	io_out8(PIC0_OCW2, 0x62);	/* IRQ-02 접수 완료를 PIC0에 통지 */
	data = io_in8(PORT_KEYDAT);
	fifo32_put(mousefifo, data + mousedata0);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec)
{
	/* write할 FIFO 버퍼를 기억 */
	mousefifo = fifo;
	mousedata0 = data0;
	/* 마우스 유효 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	/* 잘되면 ACK(0xfa)가 송신되어 온다 */
	mdec->phase = 0; /* 마우스의 0xfa를 기다리고 있는 단계 */
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
	if (mdec->phase == 0) {
		/* 마우스의 0xfa를 기다리고 있는 단계 */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		/* 마우스의 1바이트째를 기다리고 있는 단계 */
		if ((dat & 0xc8) == 0x08) {
			/* 올바른 1바이트째였다 */
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		/* 마우스의 2바이트째를 기다리고 있는 단계 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* 마우스의 3바이트째를 기다리고 있는 단계 */
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; /* 마우스에서는 y방향의 부호가 화면과 반대 */
		return 1;
	}
	return -1; /* 여기에 올 일은 없을 것 */
}
