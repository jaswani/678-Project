/*
 * sem_timer.c - Experiment with timer threads (SIGALRM)
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include "minheap.h"
#include "debug.h"

static long int i=0, flag = 0;

sem_t timer_work; /*Timer lock*/

pthread_t t; /*Timer thread*/


/* handler called when timer returns */
void handler()
{
  struct itimerval value, setvalue, ovalue;

  printf("Handler: called i = %d\n", i);

  if(i >= 2) {
      flag = 1;
      //sem_post(&timer_work);
      pthread_join(t,NULL);
      exit(1);
  }
  
  /* Could do something here like send a packet */
        
  /* setup signal SIGALRM to call handler after time is up (5 seconds) */ 
        signal(SIGALRM, handler);
        setvalue.it_interval.tv_sec=0;
        setvalue.it_interval.tv_usec=0;
        setvalue.it_value.tv_sec=5;
        setvalue.it_value.tv_usec=0;
  
        /* initialize timer */
        setitimer(ITIMER_REAL, &setvalue, &ovalue);
        printf("Restart timer with %d\n", setvalue.it_value.tv_sec); 
        i++;
  
        printf("Handler: Posting work to timer_Woker \n");
        sem_post(&timer_work); //Wake up handler thread and ask it work on heap
}

void *timer_worker(void *arg)
{
    struct itimerval value, setvalue, ovalue;
    struct itimerval val;
    while(1) {
        printf("    Worker thread waiting..\n");
        sem_wait(&timer_work);
        printf("    Got Work i = %d\n", i);
        if(i == 1) {
            i = 3;
            getitimer(ITIMER_REAL, &val);
            printf("    Getitimer val = %d\n", val.it_value.tv_sec);
            if(val.it_value.tv_sec>0 || val.it_value.tv_usec > 0){
                printf("    Setting timer to 0\n");
                signal(SIGALRM, handler);
                setvalue.it_interval.tv_sec=0;
                setvalue.it_interval.tv_usec=0;
                setvalue.it_value.tv_sec=0;
                setvalue.it_value.tv_usec=0;
  
                /* initialize timer */
                setitimer(ITIMER_REAL, &setvalue, &ovalue);
                break;
            }
            
        }

        if(flag) {
            break;
        }
    }
}

main(void)
{
  char ch;
  char junk;
  int k;

  /*Initialize timer_work semaphore with -1*/
  sem_init(&timer_work,0,-1);

  pthread_create(&t,NULL,timer_worker,(void *)i);

  /*Test code to see if timere work..*/
  /* Initiate timer */
  handler();
  
  while(1) {
  }
  
}


