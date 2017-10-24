/* 다른 파일로 만든 함수가 있으면 C컴파일러에 알려준다 */

void io_hlt(void);

/* 함수 선언인데 {}가 없고 갑자기 ;를 쓰면
	다른 파일에 있다는 의미입니다. */

void HariMain(void)
{

fin:
	io_hlt(); /* 이것으로 naskfunc.nas의 _io_hlt가 실행됩니다 */
	goto fin;

}
