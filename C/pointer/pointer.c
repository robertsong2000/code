#include <stdio.h>

int main(void)
{
	int *p = NULL;
	char *q = NULL;
	int i = 10;
	char j = 'a';
	p = &i;
	q = &j;
	printf("p:%p, q:%p\n", p, q);
	printf("p+1:%p, q+1:%p\n", p + 1, q + 1);
	return 0;
}
