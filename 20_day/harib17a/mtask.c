/* 멀티태스크 관계 */

#include "bootpack.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;

struct TASK *task_now(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

void task_add(struct TASK *task)
{
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2; /* 동작중 */
	return;
}

void task_remove(struct TASK *task)
{
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];

	/* task가 어디에 있는지를 찾는다 */
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/* 여기에 있었다 */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--; 
	}
	if (tl->now >= tl->running) {
		/* now가 이상한 값이 되어 있으면 수정한다 */
		tl->now = 0;
	}
	task->flags = 1; /* sleeve중 */

	/* 옮겨 놓기 */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;
}

void task_switchsub(void)
{
	int i;
	/* 맨 위의 레벨을 찾는다 */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break; /* 발견되었다 */
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}

void task_idle(void)
{
	for (;;) {
		io_hlt();
	}
}

struct TASK *task_init(struct MEMMAN *memman)
{
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;

	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
	}
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}

	task = task_alloc();
	task->flags = 2;	/* 동작중 마크 */
	task->priority = 2; /* 0.02초 */
	task->level = 0;	/* 최고 레벨 */
	task_add(task);
	task_switchsub();	/* 레벨 설정 */
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);

	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int) &task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	task_run(idle, MAX_TASKLEVELS - 1, 1);

	return task;
}

struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* 사용중 마크 */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			task->tss.eax = 0; /* 우선 0으로 해 두기로 한다 */
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0; /* 벌써 전부 사용중 */
}

void task_run(struct TASK *task, int level, int priority)
{
	if (level < 0) {
		level = task->level; /* 레벨을 변경하지 않는다 */
	}
	if (priority > 0) {
		task->priority = priority;
	}

	if (task->flags == 2 && task->level != level) { /* 동작중의 레벨의 변경 */
		task_remove(task); /* 이것을 실행하면 flags는 1이 되므로 아래의 if도 실행된다 */
	}
	if (task->flags != 2) {
		/* sleeve로부터 깨어나는 경우 */
		task->level = level;
		task_add(task);
	}

	taskctl->lv_change = 1; /* 다음번 태스크 스위치 때에 레벨을 다시 본다 */
	return;
}

void task_sleep(struct TASK *task)
{
	struct TASK *now_task;
	if (task->flags == 2) {
		/* 동작중이라면 */
		now_task = task_now();
		task_remove(task); /* 이것을 실행하면 flags는 1이 된다 */
		if (task == now_task) {
			/* 자기 자신의 sleeve였으므로 태스크 스위치가 필요 */
			task_switchsub();
			now_task = task_now(); /* 설정한 뒤의 「현재의 태스크」를 가르쳐 준다 */
			farjmp(0, now_task->sel);
		}
	}
	return;
}

void task_switch(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change != 0) {
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
	return;
}
