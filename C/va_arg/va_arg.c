#include <stdarg.h>
#include <stdio.h>

void vararg_func( int * sum , int argc , ... ) {
  va_list list;
  int i;

  *sum = 0;

  va_start( list ,  argc );

  for( i=0 ; i<argc ; i++ ) {
    *sum += va_arg( list , int );
  }

  va_end( list );
}

int main(void) {
  int argv[3];
  int i , argc , sum;

  argc = sizeof( argv )/sizeof( argv[0] );

  for( i=0 ; i<argc ; i++ ) {
    argv[i] = i + 1;
  }

  vararg_func( &sum , argc , argv[0] , argv[1] , argv[2] );

  printf( "sum=%d\n" , sum );/* 6 */
  return 0;
}
