#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define M 1000000
#define GIANT_SIZE (200*M)

size_t g_arr[GIANT_SIZE];

int main( int argc, char **argv){   
    int i;
    for(i = 0; i<10; i++){
        printf("This should be zero: %d\n",g_arr[i]);
    }
    exit(1);
}
