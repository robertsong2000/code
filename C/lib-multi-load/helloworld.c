#include <stdio.h>
#include <unistd.h>
#include "foo.h"

int main(void)
{
	foo();
	sleep(10);
	return 0;
}
