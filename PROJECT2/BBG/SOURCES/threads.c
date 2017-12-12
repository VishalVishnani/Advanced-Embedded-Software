#include <pthread.h>
#include "threads.h"
#include <stdio.h>



void pthread_initialize(void){
  int8_t ret=1;

  ret=pthread_mutex_init(&log_mutex,NULL);
  if(ret){
    printf("\nError in log mutex init\n");
  }

  ret=pthread_mutex_init(&dec_mutex,NULL);
  if(ret){
    printf("\nError in dec mutex init\n");

  }

  ret=pthread_mutex_init(&heartbeat_mutex,NULL);
  if(ret){
    printf("\nError in heartbeat mutex init\n");

  }

  ret=pthread_cond_init(&hb_logger,NULL);
  if(ret){
    printf("\nError in logger cond init\n");
  }

  ret=pthread_cond_init(&hb_decision,NULL);
  if(ret){
    printf("\nError in decision cond init\n");
  }

  ret=pthread_cond_init(&hb_socket,NULL);
  if(ret){
    printf("\nError in socket cond init\n");
  }

  ret=pthread_create(&threads[0],NULL,log_func,NULL);
  if(ret){
    printf("\nError in pthread create 0\n");
  }


  ret=pthread_create(&threads[1],NULL,decision_func,NULL);
  if(ret){
    printf("\nError in pthread create 1\n");
  }

  ret=pthread_create(&threads[2],NULL,socket_func,NULL);
  if(ret){
    printf("\nError in pthread create 2\n");
  }

}



void cleanup_threads(void){
  int8_t ret=1;
  ret=pthread_mutex_destroy(&log_mutex);
  if(ret){
    printf("\nError in log mutex destroy\n");
  }

  ret=pthread_mutex_destroy(&dec_mutex);
  if(ret){
    printf("\nError in dec mutex destroy\n");
  }


  ret=pthread_mutex_destroy(&heartbeat_mutex);
  if(ret){
    printf("\nError in heartbeat mutex destroy\n");
  }


  ret=pthread_cond_destroy(&hb_logger);
  if(ret){
    printf("\nError in logger cond destroy\n");
  }


  ret=pthread_cond_destroy(&hb_decision);
  if(ret){
    printf("\nError in decision cond destroy\n");
  }


  ret=pthread_cond_destroy(&hb_socket);
  if(ret){
    printf("\nError in socket cond destroy\n");
  }

}


