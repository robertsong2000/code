#include <stdio.h>
#include <stdlib.h>
typedef unsigned long ulong;

int main(int argc, char *argv[])
{
    ulong n = atoi(argv[1]);
    ulong a = 1;
    ulong b = 0;
    ulong t;
    ulong i;

    for(i = 0; i < n; i++) {
        t = b;
        b = a+b;
        a = t;
    }

    printf("%lu\n", b);
    return 0;
}
