void api_putchar(int c);
void api_end(void);

void HariMain(void)
{
	char a[100];
	a[10] = 'A';		/* 이것은 물론 좋다 */
	api_putchar(a[10]);
	a[102] = 'B';		/* 이것은 안되지요 */
	api_putchar(a[102]);
	a[123] = 'C';		/* 이것도 안되지요 */
	api_putchar(a[123]);
	api_end();
}
