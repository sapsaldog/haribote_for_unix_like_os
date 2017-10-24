#include "apilib.h"

void HariMain(void)
{
	int i, timer;
	timer = api_alloctimer();
	api_inittimer(timer, 128);
	for (i = 20000000; i >= 20000; i -= i / 100) {
		/* 20 KHz~20Hz : 인간에게 들리는 소리의 범위 */
		/* i는 1%씩 줄여간다 */
		api_beep(i);
		api_settimer(timer, 1);		/* 0.01초 */
		if (api_getkey(1) != 128) {
			break;
		}
	}
	api_beep(0);
	api_end();
}
