#include <stdio.h>

struct S1{
    char a;
    long b;
};

struct S2{
    char c;
    struct S1 d;
    long long e;
};

int main(void) {
	printf ( "size of struct S2 = %d\n", sizeof(struct S2));
	return 0;
}
