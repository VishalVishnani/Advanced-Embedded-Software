#include <stdio.h>
#include <mqueue.h>
#include "log.h"
#include "threads.h"
#include "queue.h"
#include <stdlib.h>
#include <string.h>

void* socket_func(void* t){
  log_packet recv;
  static int i=0;
  int data=0;
  time_t curtime=0;
  int8_t ret=0;

  while(1){

    memset(&recv,0,sizeof(log_packet));
    pthread_cond_broadcast(&hb_socket);

    if(exit_flag==1){
      break;
    }
    else{
      if((i%2)==0){
        recv.level=SENSOR_DATA;
        recv.task_ID=TEMP_TASK;
        strcpy(recv.log_message,"TEMP DATA");
        data=rand()%50;
        sprintf(recv.data,"%d",data);
        curtime=time(NULL);
        recv.current_time=ctime(&curtime);
      }
      else{
        recv.level=SENSOR_DATA;
        recv.task_ID=LIGHT_TASK;
        strcpy(recv.log_message,"LIGHT DATA");
        data=rand()%2000;
        sprintf(recv.data,"%d",data);
        curtime=time(NULL);
        recv.current_time=ctime(&curtime);
      }
      i++;
      if(exit_flag==1){
        break;
      }

      ret=pthread_mutex_lock(&log_mutex);
      if(ret){
        printf("\nError: log mutex lock failed in socket\n");
      }

      if(mq_send(mqdes_logger,(const int8_t*)&recv,sizeof(recv),0)==-1){
        printf("\nError in mq_send logger in socket func\n");
        exit(1);
      }

      ret=pthread_mutex_unlock(&log_mutex);
      if(ret){
        printf("\nError: log mutex unlock failed in socket\n");
      }

      if(exit_flag==1){
        break;
      }

      ret=pthread_mutex_lock(&dec_mutex);
      if(ret){
        printf("\nError: dec mutex lock failed in socket\n");
      }

      if(mq_send(mqdes_decision,(const int8_t*)&recv,sizeof(recv),0)==-1){
        printf("\nError in mq_send dec in socket func\n");
        exit(1);
      }

      ret=pthread_mutex_unlock(&dec_mutex);
      if(ret){
        printf("\nError: dec mutex unlock failed in socket\n");
      }
      sleep(1);
    }
  }
  printf("\nEnd of socket func\n");
  pthread_exit(NULL);
}

