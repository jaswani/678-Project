#ifndef _TIMER_UTILS_H
#define _TIMER_UTILS_H

int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);
int timeval_add (struct timeval *result, struct timeval *x, struct timeval *y);
int timeval_compare (struct timeval *x, struct timeval *y);

#endif
