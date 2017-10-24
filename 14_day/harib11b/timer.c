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
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	timerctl.using++;
	if (timerctl.using == 1) {
		/* 동작중의 타이머가 1개의 경우 */
		timerctl.t0 = timer;
		timer->next = 0; /* 다음은 없다 */
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	t = timerctl.t0;
	if (timer->timeout <= t->timeout) {
		/* 선두에 들어갈 수 있는 경우 */
		timerctl.t0 = timer;
		timer->next = t; /* 다음은 t */
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	/* 어디에 들어가면 될지 찾는다 */
	for (;;) {
		s = t;
		t = t->next;
		if (t == 0) {
			break; /* 제일 뒤가 되었다 */
		}
		if (timer->timeout <= t->timeout) {
			/* s와 t의 사이에 들어갈 수 있는 경우 */
			s->next = timer; /* s의 다음은 timer */
			timer->next = t; /* timer의 다음은 t */
			io_store_eflags(e);
			return;
		}
	}
	/* 제일 뒤에 들어갈 수 있는 경우 */
	s->next = timer;
	timer->next = 0;
	io_store_eflags(e);
	return;
}

void inthandler20(int *esp)
{
	int i;
	struct TIMER *timer;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00 접수 완료를 PIC에 통지 */
	timerctl.count++;
	if (timerctl.next > timerctl.count) {
		return;
	}
	timer = timerctl.t0; /* 우선 선두의 번지를 timer에 대입 */
	for (i = 0; i < timerctl.using; i++) {
		/* timers의 타이머는 모두 동작중의 것이므로, flags를 확인하지 않는다 */
		if (timer->timeout > timerctl.count) {
			break;
		}
		/* 타임 아웃 */
		timer->flags = TIMER_FLAGS_ALLOC;
		fifo32_put(timer->fifo, timer->data);
		timer = timer->next; /* 다음의 타이머의 번지를 timer에 대입 */
	}
	timerctl.using -= i;

	/* 새롭게 옮겨 놓기 */
	timerctl.t0 = timer;

	/* timerctl.next의 설정 */
	if (timerctl.using > 0) {
		timerctl.next = timerctl.t0->timeout;
	} else {
		timerctl.next = 0xffffffff;
	}
	return;
}
