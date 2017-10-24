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
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0; /* 미사용 */
	}
	t = timer_alloc(); /* 1개 받아 온다 */
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0; /* 제일 뒤 */
	timerctl.t0 = t; /* 지금은 sentinel 밖에 없기 때문에 선두이기도 하다 */
	timerctl.next = 0xffffffff; /* sentinel 밖에 없기 때문에 sentinel 시각 */
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == 0) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			timerctl.timers0[i].flags2 = 0;
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
		if (timer->timeout <= t->timeout) {
			/* s와 t의 사이에 들어갈 수 있는 경우 */
			s->next = timer; /* s의 다음은 timer */
			timer->next = t; /* timer의 다음은 t */
			io_store_eflags(e);
			return;
		}
	}
}

void inthandler20(int *esp)
{
	struct TIMER *timer;
	char ts = 0;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00 접수 완료를 PIC에 통지 */
	timerctl.count++;
	if (timerctl.next > timerctl.count) {
		return;
	}
	timer = timerctl.t0; /* 우선 선두의 번지를 timer에 대입 */
	for (;;) {
		/* timers의 타이머는 모두 동작중의 것이므로, flags를 확인하지 않는다 */
		if (timer->timeout > timerctl.count) {
			break;
		}
		/* 타임 아웃 */
		timer->flags = TIMER_FLAGS_ALLOC;
		if (timer != task_timer) {
			fifo32_put(timer->fifo, timer->data);
		} else {
			ts = 1; /* task_timer가 타임 아웃 했다 */
		}
		timer = timer->next; /* 다음 타이머의 번지를 timer에 대입 */
	}
	timerctl.t0 = timer;
	timerctl.next = timer->timeout;
	if (ts != 0) {
		task_switch();
	}
	return;
}

int timer_cancel(struct TIMER *timer)
{
	int e;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();	/* 설정중에 타이머 상태가 변화하지 않게 하기 위해 */
	if (timer->flags == TIMER_FLAGS_USING) {	/* 취소 처리는 필요한가?  */
		if (timer == timerctl.t0) {
			/* 선두였던 경우의 취소 처리 */
			t = timer->next;
			timerctl.t0 = t;
			timerctl.next = t->timeout;
		} else {
			/* 선두 이외의 경우의 취소 처리 */
			/* timer의 1개 앞을 찾는다 */
			t = timerctl.t0;
			for (;;) {
				if (t->next == timer) {
					break;
				}
				t = t->next;
			}
			t->next = timer->next; /* 「timer 직전」의 다음이, 「timer 다음」을 가리키도록 한다 */
		}
		timer->flags = TIMER_FLAGS_ALLOC;
		io_store_eflags(e);
		return 1;	/* 캔슬 처리 성공 */
	}
	io_store_eflags(e);
	return 0; /* 캔슬 처리는 불필요했다 */
}

void timer_cancelall(struct FIFO32 *fifo)
{
	int e, i;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();	/* 설정중에 타이머 상태가 변화하지 않게 하기 위해 */
	for (i = 0; i < MAX_TIMER; i++) {
		t = &timerctl.timers0[i];
		if (t->flags != 0 && t->flags2 != 0 && t->fifo == fifo) {
			timer_cancel(t);
			timer_free(t);
		}
	}
	io_store_eflags(e);
	return;
}
