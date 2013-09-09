#include <stdio.h>

int main()
{
    char c1 = 0xff, c2 = 0x01;
    printf((c1 > c2) ? "c1 > c2\n" : "c1 <= c2\n");
    return 0;
}
