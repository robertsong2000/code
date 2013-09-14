#include <unistd.h>
int i = 0;

int main()
{
    while (1) {
        sleep(5);
	i = i + 1;
    }
    return 0;
}
