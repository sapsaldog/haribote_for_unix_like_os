/* bootpack의 메인 */

#include "bootpack.h"
#include <stdio.h>

#define KEYCMD_LED		0xed

int keywin_off(struct SHEET *key_win, struct SHEET *sht_win, int cur_c, int cur_x);
int keywin_on(struct SHEET *key_win, struct SHEET *sht_win, int cur_c);

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct SHTCTL *shtctl;
	char s[40];
	struct FIFO32 fifo, keycmd;
	int fifobuf[128], keycmd_buf[32];
	int mx, my, i, cursor_x, cursor_c;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_cons[2];
	struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_cons[2];
	struct TASK *task_a, *task_cons[2];
	struct TIMER *timer;
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',   0,   '\\', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', '@', '#', '$', '%', '&', '^', '*', '(', ')', '_', '+', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0,   '|', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
	};
	int key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	struct CONSOLE *cons;
	int j, x, y, mmx = -1, mmy = -1;
	struct SHEET *sht = 0, *key_win;

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC의 초기화가 끝났으므로 CPU의 인터럽트 금지를 해제 */
	fifo32_init(&fifo, 128, fifobuf, 0);
	init_pit();
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8); /* PIT와 PIC1와 키보드를 허가(11111000) */
	io_out8(PIC1_IMR, 0xef); /* 마우스를 허가(11101111) */
	fifo32_init(&keycmd, 32, keycmd_buf, 0);

	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	*((int *) 0x0fe4) = (int) shtctl;

	/* sht_back */
	sht_back  = sheet_alloc(shtctl);
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1); /* 투명색없음 */
	init_screen8(buf_back, binfo->scrnx, binfo->scrny);

	/* sht_cons */
	for (i = 0; i < 2; i++) {
		sht_cons[i] = sheet_alloc(shtctl);
		buf_cons[i] = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
		sheet_setbuf(sht_cons[i], buf_cons[i], 256, 165, -1); /* 