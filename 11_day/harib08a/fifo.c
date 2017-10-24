/* FIFO 라이브러리 */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf)
/* FIFO 버퍼의 초기화 */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* 빈 영영 */
	fifo->flags = 0;
	fifo->p = 0; /* write 위치 */
	fifo->q = 0; /* read 위치 */
	return;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data)
/* FIFO에 데이터를 보내 쌓는다 */
{
	if (fifo->free == 0) {
		/* 빈 영역이 없어서 넘쳤다 */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--; 
	return 0;
}

int fifo8_get(struct FIFO8 *fifo)
/* FIFO로부터 데이터를 1개 가져온다 */
{
	int data;
	if (fifo->free == fifo->size) {
		/* 버퍼가 비어있을 때는 우선 -1이 주어진다 */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo8_status(struct FIFO8 *fifo)
/* 어느 정도 데이터가 모여 있을까를 보고한다 */
{
	return fifo->size - fifo->free;
}
