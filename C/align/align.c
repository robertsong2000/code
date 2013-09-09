#include <stdio.h>

int main () {
    char *str = "\x01\x23\x45\x67\x89\xab\xcd\xef";
    unsigned *u = (unsigned *)(str + 1);
    printf ( "%08x\n", *u );
    return 0;
}
