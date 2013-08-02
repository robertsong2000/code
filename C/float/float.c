#include <math.h>

int foo(void)
{
	double i = 0.5;
	double j = 0.3;
	double k;
	int l = 0;
	k = i + j;
	k = i - j;
	k = i * j;
	k = i / j;
	k = sqrt(i);
	k = i + l;
	k = fabs(i);
	return 0;
}

int bar(void)
{
	float i = 0.5;
	float j = 0.3;
	float k;
	int l = 0;
	k = i + j;
	k = i - j;
	k = i * j;
	k = i / j;
	k = sqrt(i);
	k = i + l;
	k = fabs(i);
	return 0;
}

int main(void)
{
	foo();
	bar();
	return 0;
}
