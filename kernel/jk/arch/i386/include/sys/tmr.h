#ifndef __TMR_H
#define __TMR_H


#include <sys/types.h>


u_int           tmrcount(u_int freq);
u_int           tmrtick();
void            tmrstart(u_int count);
u_int           tmrread();


#endif
