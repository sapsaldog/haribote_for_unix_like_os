#include <stdio.h>
#include "apilib.h"

#define MAX		1000

void HariMain(void)
{
	char flag[MAX], s[8];
	int i, j;
	for (i = 0; i < MAX; i++) {
		flag[i] = 0;
	}
	for (i = 2; i < MAX; i++) {
		if (flag[i] == 0) {
			/* 플래그가 세트되어 있지 않기 때문에 소수다!  */
			sprintf(s, "%d ", i);
			api_putstr0(s);
			for (j = i * 2; j < MAX; j += i) {
				flag[j] = 1;	/* 배수에는 플래그를 세트한다 */
			}
		}
	}
	api_end();
}
