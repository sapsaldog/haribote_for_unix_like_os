#include "apilib.h"

char buf[150 * 50];

void HariMain(void)
{
	int win;
	win = api_openwin(buf, 150, 50, -1, "hello");
	api_boxfilwin(win,  8, 36, 141, 43, 3 /* ³ë¶û */);
	api_putstrwin(win, 28, 28, 0 /* °ËÁ¤ */, 12, "hello, world");
	api_end();
}
