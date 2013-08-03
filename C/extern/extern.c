#include <stdio.h>

extern int foo;
extern void bar(void);

int main(void)
{
	bar();
	printf("foo is:%d\n", foo);
	return 0;
}
