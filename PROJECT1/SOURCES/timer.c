#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include "timer.h"




void timer_handler(int signum);


void timer_init(void){

  memset(&sa,0,sizeof(sa));
  sa.sa_handler=&timer_handler;

  sigaction(SIGALRM,&sa,NULL);

  timer.it_value.tv_sec=0;
  timer.it_value.tv_usec=200000;

  timer.it_interval.tv_sec=0;
  timer.it_interval.tv_usec=200000;

  setitimer(ITIMER_REAL, &timer, NULL);
}

struct timespec heartbeat_init(uint32_t sec_value,uint32_t nsec_value){
  struct timespec t1;
  struct timeval t2;
  
  gettimeofday(&t2,NULL);

  t1.tv_sec=t2.tv_sec;
  t1.tv_nsec=(t2.tv_usec)*1000;
  t1.tv_sec+=sec_value;
  t1.tv_nsec+=nsec_value;

  return t1;
}
