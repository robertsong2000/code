#include <unistd.h>

#define BAR
#include "foo.c"

int main(void)
{
	sleep(1);
	return 0;
}
