#include <stdio.h>
#include <limits.h>

int idiv(int x, int y) 
{
    return x / y;
}

int main()
{
    int x0, y0;

    x0 = 101; y0 = 3;
    printf("%d / %d = %d\n", x0, y0, idiv(x0, y0));

    x0 = INT_MIN; y0 = -1;
    printf("%d / %d = %d\n", x0, y0, idiv(x0, y0));

    x0 = 101; y0 = 0;
    printf("%d / %d = %d\n", x0, y0, idiv(x0, y0));

}
