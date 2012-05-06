/*
 * sem_timer.c - Experiment with timer threads (SIGALRM)
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include "dll.h"
#include "timer_utils.h"
#include "debug.h"

void handler();
sem_t timer_work; /*Timer lock*/
sem_t dll_lock; /*Heap lock*/

pthread_t t; /*Timer thread*/

int timer_enqueue(struct timeval *t)
{
    struct timeval min, diff;
    struct itimerval value, setvalue, ovalue;
    DEBUG_NA("\n");
    /*Wait on dll lock - right to modify the list*/
    sem_wait(&dll_lock);
    /*Add struct timeval to heap*/
    DEBUG_NA("Got dll_lock\n");
    if(!dll_add_key(t)) {
        DEBUG_NA("add_key failed");
        sem_post(&dll_lock);
        return 0;
    }

    dll_min_key(&min);

    if(!timeval_compare(t,&min)) {
        /* 
         * Either this is the first node added or key added was minimum
         */
        if(first == last) {
            //only element in the list
            //print_time("Base", &base);
            DEBUG_NA("Start timer with:");
            print_time("Enqueue first", t);
            signal(SIGALRM, handler);
            start_timer(t); 
        } else {
            struct timeval cur_time;
            struct itimerval rem_time;
            getitimer(ITIMER_REAL, &rem_time);
            stop_timer(); 
            timeval_subtract(&cur_time, &first->right->time, &rem_time.it_value);
            if(timeval_compare(t,&cur_time) == 1) {
                timeval_subtract(&diff, t, &cur_time);
                DEBUG_NA("Start timer with:");
                print_time("Enqueue", &diff);
                signal(SIGALRM, handler);
                start_timer(&diff);
            } else {
                //cant add this timer, it expired alredy!
                sem_post(&dll_lock);
                return 0;
            }
        }
    }
    sem_post(&dll_lock); //FIXME:move it above if block, after add_key
}

int timer_dequeue(node *t)
{
    struct itimerval rem_time; 
    struct timeval diff, cur_time;
    int timer_stopped = 0;

    sem_wait(&dll_lock);
    if(t == first) {
        /*if timer is expired return*/
        getitimer(ITIMER_REAL, &rem_time); 
        if(rem_time.it_value.tv_sec == 0 && rem_time.it_value.tv_usec == 0) {
            //expired timer
            sem_post(&dll_lock);
            return 1;
        }
        /* Stop the timer */ 
        stop_timer(); 
        timer_stopped = 1;
        timeval_subtract(&cur_time, &first->time, &rem_time.it_value);
        timeval_subtract(&diff, &first->right->time, &cur_time);
    }

    
    if(dll_remove_key(t) != 1) {
        DEBUG_NA("Remove key failed\n");
    }
    sem_post(&dll_lock);

    if(timer_stopped) {
        //restart timer
        signal(SIGALRM, handler);
        start_timer(&diff);
        printf("Restart timer with "); 
        print_time("Dequeue", &diff);
    }
}

/* handler called when timer returns */
void handler()
{

  printf("Handler: calld with \n");

  /* Could do something here like send a packet */
  sem_post(&timer_work); //Wake up handler thread and ask it work on heap
}

void *timer_worker()
{
    struct timeval diff, cur_time;
    
    while(1) {
        printf("Worker thread waiting..\n");
        sem_wait(&timer_work);
        printf("Got Work\n");
        
        /*Handler got invoked now delete min from heap*/
        sem_wait(&dll_lock);
        struct timeval t; 
        dll_extract_min(&t);
        if(!first) {
            //List emplty
            sem_post(&dll_lock);
            return;
        }
        timeval_subtract(&diff, &first->time, &t);
        sem_post(&dll_lock);
        signal(SIGALRM, handler);
        start_timer(&diff);
        printf("Restart timer with "); 
        print_time("Expire", &diff);
    }
}

main(int argc, char *argv[])
{
  int i, iterations;
  struct timeval value, res, base, nbase;
  struct timezone tzp;

  if(argc == 1) {
    printf("Usage ./a.out <no of elements> \n");
    return;
  } else {
    iterations = atoi(argv[1]);
    DEBUG("iterations =  %d\n", iterations);
  }

  /*Initialize timer_work semaphore with -1*/
  sem_init(&timer_work,0,0);
  /*Initialize dll_lock 0 (available)*/
  sem_init(&dll_lock,0,1);

  pthread_create(&t,NULL,timer_worker,NULL);

  /*Test code to see if timere work..*/

  value.tv_sec = 1;
  value.tv_usec = 0;
  
  printf("Inserting: \n");

  for(i=0; i<iterations;i++) {
    if(i == 0) {
        gettimeofday (&base, &tzp);
        print_time("Base", &base);
        nbase.tv_sec = base.tv_sec;
        nbase.tv_usec = base.tv_usec;
    } else {
        gettimeofday (&nbase, &tzp);
    }
    timeval_subtract(&res, &nbase, &base);
    print_time("Base", &base);
    timeval_add(&res, &res, &value);
    print_time("Adding", &res);
    timer_enqueue(&res);
    
    DEBUG_NA("Going to sleep \n");
    sleep(2);
  }
  while(1) {}
  printf("\n");
}


