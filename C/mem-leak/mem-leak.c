#include <stdlib.h>

int foo(void)
{
	malloc(1024 * 1024);
}

int main(void)
{
	foo();
	return 0;
}
