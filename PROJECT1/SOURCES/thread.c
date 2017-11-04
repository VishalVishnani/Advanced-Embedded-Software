#include "thread.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

void *func1(void* t){

  log* packet_temp=(log*)malloc(sizeof(log));
  request_log* req_temp=(request_log*)malloc(sizeof(request_log));

  int8_t ret=-1;

  time_t curtime=time(NULL);
  uint32_t count_req=0;

  while(1){

    if(destroy_all==1){
      printf("\nDestroy 1\n");
      break;
    }

    packet_temp->log_level=SENSOR_VALUE;
    packet_temp->log_id=TEMP_TASK;

    ret=pthread_mutex_lock(&temp_w_lock);
    if(ret){
      printf("\nError: mutex lock failed in func1\n");
    }

    pthread_cond_wait(&timer_expire,&temp_w_lock);


    if(cond_type==2){
      printf("\n----------------ASYNC QUERY FOR TEMP---------------\n");
      mq_getattr(mqdes_temp_r,&attr_temp_r);
      count_req=attr_temp_r.mq_curmsgs;
      printf("\nMessages currently on the temp_r queue %ld\n",attr_temp_r.mq_curmsgs);

      while(count_req){
        int n;
        n=mq_receive(mqdes_temp_r,(int8_t*)&req_temp,4096,NULL);
        if(n==0 || n<0){
          printf("\nNo message\n");
        }

        printf("\n\nSRC_ID = %d\n",req_temp->src_id);
        printf("\nDST_ID = %d\n",req_temp->dst_id);
        printf("\nTIMESTAMP = %s\n",req_temp->timestamp);
        printf("\nMESSAGE = %s\n\n",req_temp->message);


        mq_getattr(mqdes_temp_r,&attr_temp_r);
        count_req=attr_temp_r.mq_curmsgs;
      }

    }

    else if(cond_type==1){
      printf("\nWriting to temp queue_w\n");
      char buffer[]="TEMP";

      packet_temp->log_message=buffer;
      packet_temp->timestamp=ctime(&curtime);

      if(mq_send(mqdes_temp_w,(const int8_t*)&packet_temp,sizeof(packet_temp),0)==-1){
        printf("\nError in mq_send temp\n");
        exit(1);
      }

    }
    ret=pthread_mutex_unlock(&temp_w_lock);
    if(ret){
      printf("\nError: mutex unlock failed in func1\n");
    }

  }

  free(packet_temp);
  free(req_temp);
  printf("\nExiting Func 1\n");
  pthread_exit(NULL);
}



void *func3(void* t){

  char logger_level[3][15]={"INFO","ERROR","SENSOR_VALUE"};
  char task_no[5][15]={"MAIN_TASK","TEMP_TASK","LIGHT_TASK","LOGGER_TASK","DECISION_TASK"};
  char logger_level_write[15]={'\0'};
  char task_write[15]={'\0'};
  char buffer[100]={'\0'};

  uint32_t count_msg_temp=0;
  uint32_t count_msg_light=0;
  uint32_t count_main=0;
  log* packet_recv_temp=(log*)malloc(sizeof(log));
  log* packet_recv_light=(log*)malloc(sizeof(log));
  log* packet_recv_main=(log*)malloc(sizeof(log));

  int8_t ret=-1;
  int8_t n=0;

  while(1){

    ret=-1;

    //change for main lock
    ret=pthread_mutex_trylock(&main_queue_lock);
    if(ret==0){

      pthread_cond_wait(&timer_expire,&main_queue_lock);
    

      mq_getattr(mqdes_main,&attr_main);
      count_main=attr_main.mq_curmsgs;
      printf("\nMessages currently on the main queue %ld\n",attr_temp_w.mq_curmsgs);

      while(count_main){
        n=mq_receive(mqdes_main,(int8_t*)&packet_recv_main,4096,NULL);
        if(n==0 || n<0){
          printf("\nNo message\n");
        }

      
        strcpy(logger_level_write, logger_level[packet_recv_main->log_level]);
        strcpy(task_write, task_no[packet_recv_main->log_id]);
        sprintf(buffer,"\nLOG LEVEL    %s\nTASK ID      %s\nTIMESTAMP    %sLOG MESSAGE  %s\n",logger_level_write,task_write,packet_recv_main->timestamp,packet_recv_main->log_message);
       
        printf("%s\n",buffer);
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



    ret=pthread_mutex_trylock(&temp_w_lock);
    if(ret==0){
 
      pthread_cond_wait(&timer_expire,&temp_w_lock);
 

      mq_getattr(mqdes_temp_w,&attr_temp_w);
      count_msg_temp=attr_temp_w.mq_curmsgs;
      printf("\nMessages currently on the temp_w queue %ld\n",attr_temp_w.mq_curmsgs);

      while(count_msg_temp){
        n=mq_receive(mqdes_temp_w,(int8_t*)&packet_recv_temp,4096,NULL);
        if(n==0 || n<0){
          printf("\nNo message\n");
        }
        

        strcpy(logger_level_write, logger_level[packet_recv_temp->log_level]);
        strcpy(task_write, task_no[packet_recv_temp->log_id]);
        sprintf(buffer,"\nLOG LEVEL    %s\nTASK ID      %s\nTIMESTAMP    %sLOG MESSAGE  %s\n",logger_level_write,task_write,packet_recv_temp->timestamp,packet_recv_temp->log_message);
        
        FILE* fd=fopen(file_open,"a+");
        if(fd==NULL){
          printf("\nERROR: file open failed\n");
        }

        fwrite(buffer,1,strlen(buffer),fd);
        fclose(fd);

        printf("%s\n",buffer);

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
      pthread_cond_wait(&timer_expire,&light_w_lock);
    
      mq_getattr(mqdes_light_w,&attr_light_w);
      count_msg_light=attr_light_w.mq_curmsgs;
      printf("\nMessages currently on the light queue_w %ld\n",attr_light_w.mq_curmsgs);

      while(count_msg_light){
        n=mq_receive(mqdes_light_w,(int8_t*)&packet_recv_light,4096,NULL);
        

        strcpy(logger_level_write, logger_level[packet_recv_light->log_level]);
        strcpy(task_write, task_no[packet_recv_light->log_id]);
        sprintf(buffer,"\n\nLOG LEVEL    %s\nTASK ID      %s\nTIMESTAMP    %sLOG MESSAGE  %s\n\n",logger_level_write,task_write,packet_recv_light->timestamp,packet_recv_light->log_message);

        FILE* fd=fopen(file_open,"a+");
        if(fd==NULL){
          printf("\nERROR: file open failed\n");
        }

        fwrite(buffer,1,strlen(buffer),fd);
        fclose(fd);

        printf("%s\n",buffer);
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
    if(destroy_all==1){
      printf("\nDestroy 3\n");
      break;
    }
    
  }
  

  free(packet_recv_temp);
  free(packet_recv_light);
  free(packet_recv_main);


  printf("\nExiting Func 3\n");
  pthread_exit(NULL);

}



void *func2(void* t){

  log* packet_light=(log*)malloc(sizeof(log));
  request_log* req_light=(request_log*)malloc(sizeof(log));
  int8_t ret=-1;

  time_t curtime=time(NULL);
  uint32_t count_req=0;

  while(1){
    if(destroy_all==1){
      printf("\nDestroy 2\n");
      break;
    }

    packet_light->log_level=SENSOR_VALUE;
    packet_light->log_id=LIGHT_TASK;

    ret=pthread_mutex_lock(&light_w_lock);
    if(ret){
      printf("\nError: mutex lock failed in func2\n");
    }

    pthread_cond_wait(&timer_expire,&light_w_lock);

    if(cond_type==3){
      printf("\n----------------ASYNC QUERY FOR LIGHT----------------------\n");
      mq_getattr(mqdes_light_r,&attr_light_r);
      count_req=attr_light_r.mq_curmsgs;
      printf("\nMessages currently on the light_r queue %ld\n",attr_light_r.mq_curmsgs);

      while(count_req){
        int n;
        n=mq_receive(mqdes_light_r,(int8_t*)&req_light,4096,NULL);
        if(n==0 || n<0){
          printf("\nNo message\n");
        }

        printf("\n\nSRC_ID = %d\n",req_light->src_id);
        printf("\nDST_ID = %d\n",req_light->dst_id);
        printf("\nTIMESTAMP = %s\n",req_light->timestamp);
        printf("\nMESSAGE = %s\n\n",req_light->message);


        mq_getattr(mqdes_light_r,&attr_light_r);
        count_req=attr_light_r.mq_curmsgs;
      }

    }


    else if(cond_type==1){
      printf("\nWriting data in light queue_w\n");
      char buffer[]="LIGHT";
      packet_light->log_message=buffer;
      packet_light->timestamp=ctime(&curtime);

      if(mq_send(mqdes_light_w,(const int8_t*)&packet_light,sizeof(packet_light),0)==-1){
        printf("\nError in mq_send light\n");
        exit(1);
      }
    }
      ret=pthread_mutex_unlock(&light_w_lock);
      if(ret){
        printf("\nError: mutex unlock failed in func2\n");
      }
      sleep(1);
      printf("\nMutex unlock for light\n");

  }
 
  free(packet_light);
  free(req_light);

  printf("\nExiting Func 2\n");
  pthread_exit(NULL);

}


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



  ret=pthread_cond_init(&timer_expire,NULL);
  if(ret){
    printf("\nError in cond init\n");
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

}

