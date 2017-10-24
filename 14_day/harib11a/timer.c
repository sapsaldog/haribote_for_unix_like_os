/* 타이머 관계 */

#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

#define TIMER_FLAGS_ALLOC		1	/* 확보한 상태 */
#define TIMER_FLAGS_USING		2	/* 타이머 작동중 */

void init_pit(void)
{
	int i;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.next = 0xffffffff; /* 처음에는 작동중의 타이머가 없기 때문에 */
	timerctl.using = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0; /* 미사용 */
	}
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == 0) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0; /* 발견되지 않았다 */
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0; /* 미사용 */
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int e, i, j;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	/* 어디에 들어가면 될지 찾는다 */
	for (i = 0; i < timerctl.using; i++) {
		if (timerctl.timers[i]->timeout >= timer->timeout) {
			break;
		}
	}
	/* 뒤로 옮겨 놓는다 */
	for (j = timerctl.using; j > i; j--) {
		timerctl.timers[j] = timerctl.timers[j - 1];
	}
	timerctl.using++;
	timerctl.timers[i] = timer;
	timerctl.next = timerctl.timers[0]->timeout;
	io_store_eflags(e);
	return;
}

void inthandler20(int *esp)
{
	int i, j;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00 접수 완료를 PIC에 통지 */
	timerctl.count++;
	if (timerctl.next > timerctl.count) {
		return;
	}
	for (i = 0; i < timerctl.using; i++) {
		/* timers의 타이머는 모두 동작중의 것이므로, flags를 확인하지 않는다 */
		if (timerctl.timers[i]->timeout > timerctl.count) {
			break;
		}
		/* 타임 아웃 */
		timerctl.timers[i]->flags = TIMER_FLAGS_ALLOC;
		fifo32_put(timerctl.timers[i]->fifo, timerctl.timers[i]->data);
	}
	/* 정확히 i개의 타이머가 타임 아웃 했다.나머지를 옮겨 놓는다. */
	timerctl.using -= i;
	for (j = 0; j < timerctl.using; j++) {
		timerctl.timers[j] = timerctl.timers[i + j];
	}
	if (timerctl.using > 0) {
		timerctl.next = timerctl.timers[0]->timeout;
	} else {
		timerctl.next = 0xffffffff;
	}
	return;
}
