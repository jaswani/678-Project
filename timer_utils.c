/*
 * timer_utils.c - working with gnu c timer structures
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include "minheap.h"
#include "debug.h"

struct itimerval value, setvalue, ovalue;

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  
   
   http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 */

int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

int timeval_add (struct timeval *result, struct timeval *x, struct timeval *y)
{
  int nsec = (x->tv_usec + y->tv_usec)/1000000; 

  /* Compute the sum.*/
  result->tv_sec = x->tv_sec + y->tv_sec + nsec;
  result->tv_usec = (x->tv_usec + y->tv_usec) % 1000000;

  /* Return 1. */
  return 1;
}

int timeval_compare (struct timeval *x, struct timeval *y)
{
  if(x->tv_sec > y->tv_sec) {
      return 1;
  } else if(x->tv_sec < y->tv_sec) {
      return -1;
  } else {
      if(x->tv_usec > y->tv_usec) {
          return 1;
      } else if(x->tv_usec < y->tv_usec) {
          return -1;
      } else {
          return 0;
      }
  }
}

void start_timer(struct timeval *t)
{
    struct itimerval setvalue, ovalue;
    setvalue.it_interval.tv_sec=0;
    setvalue.it_interval.tv_usec=0;
    setvalue.it_value.tv_sec=t->tv_sec;
    setvalue.it_value.tv_usec=t->tv_usec;
  
    /* initialize timer */
    setitimer(ITIMER_REAL, &setvalue, &ovalue);
 }
void stop_timer()
{
    struct itimerval setvalue, ovalue;
    setvalue.it_interval.tv_sec=0;
    setvalue.it_interval.tv_usec=0;
    setvalue.it_value.tv_sec=0;
    setvalue.it_value.tv_usec=0;
  
    /* initialize timer */
    setitimer(ITIMER_REAL, &setvalue, &ovalue);
 }

void inline print_time(char *name, struct timeval *t)
{
    printf("%s: tv_sec = %d, tv_usec = %d\n", name, t->tv_sec, t->tv_usec);
}

#if 0
main(void)
{
  /*Test code to see if timers operations work..*/
   struct timeval t1, t2, result, base;
   struct timezone tzp;
   t1.tv_sec = 5;
   t1.tv_usec = 900000;
   t2.tv_sec = 3;
   t2.tv_usec = 900000;

   //timeval_add(&result, &t1, &t2);
   gettimeofday (&base, &tzp);
   printf("Base: tvsec = %lu, tv_usec = %lu\n", base.tv_sec, base.tv_usec);
   timeval_add(&result, &t1, &base);
   printf("Result: tvsec = %lu, tv_usec = %lu, compare = %d\n", result.tv_sec, result.tv_usec,
           timeval_compare(&base,&result));
   
}
#endif
