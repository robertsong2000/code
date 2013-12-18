#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void dump(void) {
     int j, nptrs;
#define SIZE 100
     void *buffer[100];
     char **strings;
     nptrs = backtrace(buffer, SIZE);
     strings = backtrace_symbols(buffer, nptrs);
     if (strings == NULL) {
	 perror("backtrace_symbols");
	 exit(EXIT_FAILURE);
     }

     for (j = 0; j < nptrs; j++)
	 printf("%s\n", strings[j]);

     free(strings);
}

int foo(void)
{
  dump();
  return 0;
}

int main(void)
{
  foo();
    return 0;
}
