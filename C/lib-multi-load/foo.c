#include "foo.h"
#include <stdio.h>

__attribute__((constructor)) static void foo_init()
{
	printf("constructor\n");
	return;
}

int foo(void)
{
	printf("foo in foo.c\n");
	return 0;
}
