#include "apilib.h"

struct DLL_STRPICENV {	/* 64KB */
	int work[64 * 1024 / 4];
};

struct RGB {
	unsigned char b, g, r, t;
};

/* bmp.nasm */
int info_BMP(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_BMP(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);

/* jpeg.c */
int info_JPEG(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_JPEG(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);

unsigned char rgb2pal(int r, int g, int b, int x, int y);
void error(char *s);

void HariMain(void)
{
	struct DLL_STRPICENV env;
	char filebuf[512 * 1024], winbuf[1040 * 805];
	char s[32], *p;
	int win, i, j, fsize, xsize, info[8];
	struct RGB picbuf[1024 * 768], *q;

	/* 커맨드 라인 해석 */
	api_cmdline(s, 30);
	for (p = s; *p > ' '; p++) { }	/* 스페이스가 올 때까지 건너 뛴다 */
	for (; *p == ' '; p++) { }	/* 스페이스를 건너 뛴다 */

	/* 파일 read */
	i = api_fopen(p); if (i == 0) { error("file not found.\n"); }
	fsize = api_fsize(i, 0);
	if (fsize > 512 * 1024) {
		error("file too large.\n");
	}
	api_fread(filebuf, fsize, i);
	api_fclose(i);

	/* 파일 타입 체크 */
	if (info_BMP(&env, info, fsize, filebuf) == 0) {
		/* BMP는 아니었다 */
		if (info_JPEG(&env, info, fsize, filebuf) == 0) {
			/* JPEG도 아니었다 */
			api_putstr0("file type unknown.\n");
			api_end();
		}
	}
	/* 어느 쪽인가의 info 함수가 성공하면, 이하의 정보가 info에 들어가 있다 */
	/*	info[0] : 파일 타입 (1:BMP, 2:JPEG) */
	/*	info[1] : 칼라 정보 */
	/*	info[2] : xsize */
	/*	info[3] : ysize */

	if (info[2] > 1024 || info[3] > 768) {
		error("picture too large.\n");
	}

	/* 윈도우의 준비 */
	xsize = info[2] + 16;
	if (xsize < 136) {
		xsize = 136;
	}
	win = api_openwin(winbuf, xsize, info[3] + 37, -1, "gview");

	/* 파일 내용을 화상 데이터로 변환 */
	if (info[0] == 1) {
		i = decode0_BMP (&env, fsize, filebuf, 4, (char *) picbuf, 0);
	} else {
		i = decode0_JPEG(&env, fsize, filebuf, 4, (char *) picbuf, 0);
	}
	/* b_type = 4는 struct RGB 형식을 의미한다 */
	/* skip은 0으로 해 두면 좋다 */
	if (i != 0) {
		error("decode error.\n");
	}

	/* 표시 */
	for (i = 0; i < info[3]; i++) {
		p = winbuf + (i + 29) * xsize + (xsize - info[2]) / 2;
		q = picbuf + i * info[2];
		for (j = 0; j < info[2]; j++) {
			p[j] = rgb2pal(q[j].r, q[j].g, q[j].b, j, i);
		}
	}
	api_refreshwin(win, (xsize - info[2]) / 2, 29, (xsize - info[2]) / 2 + info[2], 29 + info[3]);

	/* 종료 대기 */
	for (;;) {
		i = api_getkey(1);
		if (i == 'Q' || i == 'q') {
			api_end();
		}
	}
}

unsigned char rgb2pal(int r, int g, int b, int x, int y)
{
	static int table[4] = { 3, 1, 0, 2 };
	int i;
	x &= 1; /* 짝수인가 홀수인가 */
	y &= 1;
	i = table[x + y * 2];	/* 중간색을 만들기 위한 정수 */
	r = (r * 21) / 256;	/* 이것으로 0~20 이 된다 */
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	r = (r + i) / 4;	/* 이것으로 0~5 가 된다 */
	g = (g + i) / 4;
	b = (b + i) / 4;
	return 16 + r + g * 6 + b * 36;
}

void error(char *s)
{
	api_putstr0(s);
	api_end();
}
