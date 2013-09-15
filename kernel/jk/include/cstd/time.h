#ifndef __TIME_H
#define __TIME_H


#include <sys/types.h>


/*
 * XXX This is really ugly, but it works (tm).  This is a cheesy short-term
 * delay for non-Pentium computers.
 */
#define DELAY(N) { register int n; for (n = 100 * (N); n > 0; n--); }


struct tm {
    int             tm_sec;	       /* Seconds after the minute [0-60] */
    int             tm_min;	       /* Minutes after the hour [0-59] */
    int             tm_hour;	       /* Hours since midnight [0-23] */
    int             tm_mday;	       /* Day of the month [1-31] */
    int             tm_mon;	       /* Months since January [0-11] */
    int             tm_year;	       /* Years since 1900 */
    int             tm_wday;	       /* Days since Sunday [0-6] */
    int             tm_yday;	       /* Days since January 1 [0-365] */
    int             tm_isdst;	       /* Daylight Savings Time flag */
};


typedef u_long  time_t;


time_t          time();
void            time2str(const time_t clock, char *s);
void            tm2str(const struct tm * tm, char *s);
void            time2tm(const time_t clock, struct tm * tm);
time_t          tm2time(struct tm * tm);


#endif
