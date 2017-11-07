/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : thread.c
* Description : Header file for Threads
*               -func1()
*               -func2()
*               -func3()
*               -func4()
*               -pthread_initialize()
******************************************************************************************/




/*INCLUDES*/
#include "thread.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>


/*Function to initialize pthreads, mutexes, conditions*/
void pthread_initialize(void){
  int8_t ret=1;

  ret=pthread_mutex_init(&temp_w_lock,NULL);
  if(ret){
    printf("\nError in temp_w mutex init\n");
  }

  ret=pthread_mutex_init(&main_queue_lock,NULL);
  if(ret){
    printf("\nError in main mutex init\n");
  }

  ret=pthread_mutex_init(&heart_beat_temp_lock,NULL);
  if(ret){
    printf("\nError in heart_beat temp mutex init\n");
  }

  ret=pthread_mutex_init(&heart_beat_light_lock,NULL);
  if(ret){
    printf("\nError in heart_beat light mutex init\n");
  }

  ret=pthread_mutex_init(&heart_beat_logger_lock,NULL);
  if(ret){
    printf("\nError in heart_beat logger mutex init\n");
  }


  ret=pthread_mutex_init(&decision_lock,NULL);
  if(ret){
    printf("\nError in decision logger mutex init\n");
  }

  ret=pthread_mutex_init(&heart_beat_decision_lock,NULL);
  if(ret){
    printf("\nError in heartbeat decision logger mutex init\n");
  }


  ret=pthread_cond_init(&timer_expire_light,NULL);
  if(ret){
    printf("\nError in cond init\n");
  }

  ret=pthread_cond_init(&timer_expire_temp,NULL);
  if(ret){
    printf("\nError in cond init\n");
  }


  ret=pthread_cond_init(&heartbeat_temp,NULL);
  if(ret){
    printf("\nError in cond heartbeat_temp init\n");
  }

  ret=pthread_cond_init(&heartbeat_light,NULL);
  if(ret){
    printf("\nError in cond heartbeat_light init\n");
  }

  ret=pthread_cond_init(&heartbeat_logger,NULL);
  if(ret){
    printf("\nError in cond heartbeat_logger init\n");
  }

  ret=pthread_cond_init(&heartbeat_decision,NULL);
  if(ret){
    printf("\nError in cond heartbeat_decision init\n");
  }


  ret=pthread_mutex_init(&light_w_lock,NULL);
  if(ret){
    printf("\nError in light_w mutex init\n");
  }

  ret=pthread_create(&threads[0],NULL,func1,NULL);
  if(ret){
    printf("\nError in pthread create 1\n");
  }


  ret=pthread_create(&threads[1],NULL,func2,NULL);
  if(ret){
    printf("\nError in pthread create 2\n");
  }


  ret=pthread_create(&threads[2],NULL,func3,NULL);
  if(ret){
    printf("\nError in pthread create 3\n");
  }

  ret=pthread_create(&threads[3],NULL,func4,NULL);
  if(ret){
    printf("\nError in pthread create 4\n");
  }


}

