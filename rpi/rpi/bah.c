#include <stdio.h>
int main()
{
	char lol;
	lol = (char)((0xff << 8) + 0xfe);
	printf("HURR %s, %d\n", lol, sizeof lol);
}