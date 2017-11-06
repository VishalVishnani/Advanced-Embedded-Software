#include "thread.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include "logger.h"

void *func3(void* t){

  char logger_level[4][15]={"INFO","ERROR","SENSOR_VALUE","ALERT"};
  char task_no[5][15]={"MAIN_TASK","TEMP_TASK","LIGHT_TASK","LOGGER_TASK","DECISION_TASK"};

  char logger_level_write[15]={'\0'};
  char task_write[15]={'\0'};
  char buffer[300]={'\0'};

  uint32_t count_msg_temp=0;
  uint32_t count_msg_light=0;
  uint32_t count_main=0;
  log packet_recv_temp;
  log packet_recv_light;
  log packet_recv_main;

  int8_t ret=-1;
  int8_t n=0;

  while(1){

    ret=-1;
    pthread_cond_broadcast(&heartbeat_logger);

    ret=pthread_mutex_trylock(&temp_w_lock);
    if(ret==0){

      mq_getattr(mqdes_temp_w,&attr_temp_w);
      count_msg_temp=attr_temp_w.mq_curmsgs;

      while(count_msg_temp){
        n=mq_receive(mqdes_temp_w,(int8_t*)&packet_recv_temp,4096,NULL);
        

        strcpy(logger_level_write, logger_level[packet_recv_temp.log_level]);
        strcpy(task_write, task_no[packet_recv_temp.log_id]);
        sprintf(buffer,"\n\nLOG LEVEL    %s\nTASK ID      %s\nTIMESTAMP    %sLOG MESSAGE  %s\nLOG DATA     %s\n\n",logger_level_write,task_write,packet_recv_temp.timestamp,packet_recv_temp.log_message,packet_recv_temp.data);
        
        FILE* fd=fopen(file_open,"a+");
        if(fd==NULL){
          printf("\nERROR: file open failed\n");
        }

        fwrite(buffer,1,strlen(buffer),fd);
        fclose(fd);


        memset(logger_level_write,'\0',sizeof(logger_level_write));
        memset(task_write,'\0',sizeof(task_write));
        memset(buffer,'\0',sizeof(buffer));

        mq_getattr(mqdes_temp_w,&attr_temp_w);
        count_msg_temp=attr_temp_w.mq_curmsgs;
      }
      ret=pthread_mutex_unlock(&temp_w_lock);
      if(ret){
        printf("\nMutex unlock temp failed in func3\n");
      }
      mq_getattr(mqdes_temp_w,&attr_temp_w);

    }

    ret=-1;

    ret=pthread_mutex_trylock(&light_w_lock);
    if(ret==0){
    
      mq_getattr(mqdes_light_w,&attr_light_w);
      count_msg_light=attr_light_w.mq_curmsgs;

      while(count_msg_light){
        n=mq_receive(mqdes_light_w,(int8_t*)&packet_recv_light,4096,NULL);
        

        strcpy(logger_level_write, logger_level[packet_recv_light.log_level]);
        strcpy(task_write, task_no[packet_recv_light.log_id]);
        sprintf(buffer,"\n\nLOG LEVEL    %s\nTASK ID      %s\nTIMESTAMP    %sLOG MESSAGE  %s\nLOG DATA     %s\n\n",logger_level_write,task_write,packet_recv_light.timestamp,packet_recv_light.log_message,packet_recv_light.data);

        FILE* fd=fopen(file_open,"a+");
        if(fd==NULL){
          printf("\nERROR: file open failed\n");
        }

        fwrite(buffer,1,strlen(buffer),fd);
        fclose(fd);

        memset(logger_level_write,'\0',sizeof(logger_level_write));
        memset(task_write,'\0',sizeof(task_write));
        memset(buffer,'\0',sizeof(buffer));


        mq_getattr(mqdes_light_w,&attr_light_w);
        count_msg_light=attr_light_w.mq_curmsgs;
      }
      ret=pthread_mutex_unlock(&light_w_lock);
      if(ret){
        printf("\nMutex unlock light failed in func3\n");
      }
      mq_getattr(mqdes_light_w,&attr_light_w);

    }


    ret=pthread_mutex_trylock(&main_queue_lock);
    if(ret==0){
    
      mq_getattr(mqdes_main,&attr_main);
      count_main=attr_main.mq_curmsgs;
      while(count_main){
        n=mq_receive(mqdes_main,(int8_t*)&packet_recv_main,4096,NULL);
        
        printf("\n%s\n",packet_recv_main.log_message);
        strcpy(logger_level_write, logger_level[packet_recv_main.log_level]);
        strcpy(task_write, task_no[packet_recv_main.log_id]);
        sprintf(buffer,"\n\nLOG LEVEL    %s\nTASK ID      %s\nTIMESTAMP    %sLOG MESSAGE  %s\nLOG DATA    %s\n\n",logger_level_write,task_write,packet_recv_main.timestamp,packet_recv_main.log_message,packet_recv_main.data);
       
        FILE* fd=fopen(file_open,"a+");
        if(fd==NULL){
          printf("\nERROR: file open failed\n");
        }

        fwrite(buffer,1,strlen(buffer),fd);
        fclose(fd);

        memset(logger_level_write,'\0',sizeof(logger_level_write));
        memset(task_write,'\0',sizeof(task_write));        
        memset(buffer,'\0',sizeof(buffer));

        mq_getattr(mqdes_main,&attr_main);
        count_main=attr_main.mq_curmsgs;
      }
      ret=pthread_mutex_unlock(&main_queue_lock);
      if(ret){
        printf("\nMutex unlock temp failed in func3\n");
      }
      mq_getattr(mqdes_main,&attr_main);
    }

    if(destroy_all==1){
	break;
    }
    
    usleep(120000);

  }
  

  printf("\nExiting Func 3\n");
  pthread_exit(NULL);

}


