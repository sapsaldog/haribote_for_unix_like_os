/* 메모리 관계 */

#include "bootpack.h"

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 386인가,  486이후인가의 확인 */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* 386에서는 AC=1로 해도 자동으로 0으로 돌아와 버린다 */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* 캐쉬 금지 */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* 캐쉬 허가 */
		store_cr0(cr0);
	}

	return i;
}

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			/* 빈 영역 정보의 개수 */
	man->maxfrees = 0;		/* 상황 관찰용：frees의 최대치 */
	man->lostsize = 0;		/* 해방에 실패한 합계 사이즈 */
	man->losts = 0;			/* 해방에 실패한 회수 */
	return;
}

unsigned int memman_total(struct MEMMAN *man)
/* 빈 영역 사이즈의 합계를 보고 */
{
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
/* 확보 */
{
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			/* 충분한 넓이의 빈 영역을 발견 */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* free[i]가 없어졌으므로 앞으로 채운다 */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* 구조체의 대입 */
				}
			}
			return a;
		}
	}
	return 0; /* 빈 영역이 없다 */
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
/* 해방 */
{
	int i, j;
	/* 정리하기 쉽게 생각하면, free[]가 addr순서로 줄지어 있는 편이 좋다 */
	/* 그러니까 먼저 어디에 들어갈 것인가를 결정한다 */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* 앞이 있다 */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* 전의 빈 영역을 정리한다 */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* 뒤도 있다 */
				if (addr + size == man->free[i].addr) {
					/* 뒤와도 정리한다 */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]의 삭제 */
					/* free[i]가 없어졌으므로 앞으로 채운다 */
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* 구조체의 대입 */
					}
				}
			}
			return 0; /* 성공 종료 */
		}
	}
	/* 앞과는 정리하지 않았다 */
	if (i < man->frees) {
		/* 뒤가 있다 */
		if (addr + size == man->free[i].addr) {
			/* 뒤와는 정리한다 */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* 성공 종료 */
		}
	}
	/* 앞도 뒤도 정리하지 않는다 */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]보다 뒤를, 뒤로 옮겨놓고 사이를 만든다 */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* 최대치를 갱신 */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* 성공 종료 */
	}
	/* 뒤로 옮겨 놓을 수 없었다 */
	man->losts++;
	man->lostsize += size;
	return -1; /* 실패 종료 */
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
