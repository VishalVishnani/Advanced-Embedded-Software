#include "thread.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

void *func1(void* t){

  log packet_temp;
  request_log req_temp;

  int8_t ret=-1;

  time_t curtime=time(NULL);
  uint32_t count_req=0;
  check_alive=0;

  while(1){

   pthread_cond_broadcast(&heartbeat_temp);

    if(destroy_all==1){
      break;
    }

    packet_temp.log_level=SENSOR_VALUE;
    packet_temp.log_id=TEMP_TASK;

    ret=pthread_mutex_lock(&temp_w_lock);
    if(ret){
      printf("\nError: mutex lock failed in func1\n");
    }

  
      pthread_cond_wait(&timer_expire_temp,&temp_w_lock);
	
     if(destroy_all==1){
    	 ret=pthread_mutex_unlock(&temp_w_lock);
    	 if(ret){
      		printf("\nError: mutex unlock failed in func1\n");
    	 }
 
	  continue;
    }

	
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

        printf("\n\nSRC_ID = %d\n",req_temp.src_id);
        printf("\nDST_ID = %d\n",req_temp.dst_id);
        printf("\nTIMESTAMP = %s\n",req_temp.timestamp);
        printf("\nMESSAGE = %s\n\n",req_temp.message);


        mq_getattr(mqdes_temp_r,&attr_temp_r);
        count_req=attr_temp_r.mq_curmsgs;
      }

    }

    else if(cond_type==1){
      printf("\nWriting to temp queue_w\n");

      strcpy(packet_temp.log_message,"TEMPERATURE DATA");
      packet_temp.timestamp=ctime(&curtime);
      float data= (float)rand()/1000000;
      sprintf(packet_temp.data,"%f",data);

      if(mq_send(mqdes_temp_w,(const int8_t*)&packet_temp,sizeof(packet_temp),0)==-1){
        printf("\nError in mq_send temp\n");
        exit(1);
      }

    }
    ret=pthread_mutex_unlock(&temp_w_lock);
    if(ret){
      printf("\nError: mutex unlock failed in func1\n");
    }

    ret=pthread_mutex_lock(&decision_lock);
    if(ret){
      printf("\nError: mutex lock failed in func1\n");
    }

    if(mq_send(mqdes_decision,(const int8_t*)&packet_temp,sizeof(packet_temp),0)==-1){
        printf("\nError in mq_send  decision temp\n");
        exit(1);
     }

    ret=pthread_mutex_unlock(&decision_lock);
    if(ret){
      printf("\nError: mutex unlock failed in func1\n");
    }


  }

  printf("\nExiting Func 1\n");
  pthread_exit(NULL);
}



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
      printf("\nMessages currently on the temp_w queue %ld\n",attr_temp_w.mq_curmsgs);

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
    
      mq_getattr(mqdes_light_w,&attr_light_w);
      count_msg_light=attr_light_w.mq_curmsgs;
      printf("\nMessages currently on the light queue_w %ld\n",attr_light_w.mq_curmsgs);

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


    ret=pthread_mutex_trylock(&main_queue_lock);
    if(ret==0){
    
      mq_getattr(mqdes_main,&attr_main);
      count_main=attr_main.mq_curmsgs;
      printf("\nMessages currently on the main queue %ld\n",attr_main.mq_curmsgs);
      while(count_main){
        n=mq_receive(mqdes_main,(int8_t*)&packet_recv_main,4096,NULL);
        
        printf("\n%s\n",packet_recv_main.log_message);
        strcpy(logger_level_write, logger_level[packet_recv_main.log_level]);
        strcpy(task_write, task_no[packet_recv_main.log_id]);
        sprintf(buffer,"\n\nLOG LEVEL    %s\nTASK ID      %s\nTIMESTAMP    %sLOG MESSAGE  %s\nLOG DATA    %s\n\n",logger_level_write,task_write,packet_recv_main.timestamp,packet_recv_main.log_message,packet_recv_main.data);
       
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

    if(destroy_all==1){
	break;
    }
    
    usleep(120000);

  }
  

  printf("\nExiting Func 3\n");
  pthread_exit(NULL);

}



void *func2(void* t){

  log packet_light;
  request_log req_light;
  int8_t ret=-1;

  time_t curtime=time(NULL);
  uint32_t count_req=0;

  while(1){
    pthread_cond_broadcast(&heartbeat_light);
    check_alive++; //enable it to check alive timer

    if(check_alive==22){
      sleep(5);
    }

   if(destroy_all==1){
      break;
    }


    packet_light.log_level=SENSOR_VALUE;
    packet_light.log_id=LIGHT_TASK;

    ret=pthread_mutex_lock(&light_w_lock);
    if(ret){
      printf("\nError: mutex lock failed in func2\n");
    }

    pthread_cond_wait(&timer_expire_light,&light_w_lock);
    
    if(destroy_all==1){
      ret=pthread_mutex_unlock(&light_w_lock);
      if(ret){
      	printf("\nError: mutex unlock failed in func1\n");
      }
      continue;
    }

    if(cond_type==3){
      printf("\n----------------ASYNC QUERY FOR LIGHT----------------------\n");
      mq_getattr(mqdes_light_r,&attr_light_r);
      count_req=attr_light_r.mq_curmsgs;
      printf("\nMessages currently on the light_r queue %ld\n",attr_light_r.mq_curmsgs);

      while(count_req){
        int n;
        n=mq_receive(mqdes_light_r,(int8_t*)&req_light,4096,NULL);

        printf("\n\nSRC_ID = %d\n",req_light.src_id);
        printf("\nDST_ID = %d\n",req_light.dst_id);
        printf("\nTIMESTAMP = %s\n",req_light.timestamp);
        printf("\nMESSAGE = %s\n\n",req_light.message);


        mq_getattr(mqdes_light_r,&attr_light_r);
        count_req=attr_light_r.mq_curmsgs;
      }

    }


    else if(cond_type==1){
      printf("\nWriting data in light queue_w\n");
      strcpy(packet_light.log_message,"LIGHT");
      packet_light.timestamp=ctime(&curtime);
      float data= (float)rand()/1000000;
      sprintf(packet_light.data,"%f",data);

      if(mq_send(mqdes_light_w,(const int8_t*)&packet_light,sizeof(packet_light),0)==-1){
        printf("\nError in mq_send light\n");
        exit(1);
      }
    }

    ret=pthread_mutex_unlock(&light_w_lock);
    if(ret){
        printf("\nError: mutex unlock failed in func2\n");
     }
   
    ret=pthread_mutex_lock(&decision_lock);
    if(ret){
      printf("\nError: mutex lock failed in func2\n");
    }

    if(mq_send(mqdes_decision,(const int8_t*)&packet_light,sizeof(packet_light),0)==-1){
      printf("\nError in mq_send  decision light\n");
      exit(1);
    }

    ret=pthread_mutex_unlock(&decision_lock);
    if(ret){
      printf("\nError: mutex unlock failed in func2\n");
    }


  }
 

  printf("\nExiting Func 2\n");
  pthread_exit(NULL);

}

void *func4(void* t){
  log packet_recv_decision;
  log main_log_packet;
  uint32_t count_decision=0;
  int8_t n=0;
  int8_t ret=-1;
  time_t curtime;
  while(1){
    pthread_cond_broadcast(&heartbeat_decision);

    ret=-1;
    ret=pthread_mutex_lock(&decision_lock);
    if(ret){
      printf("\nError: mutex lock failed in func4\n");
    }
    n=0;

    count_decision=0;

    mq_getattr(mqdes_decision,&attr_decision);
    count_decision=attr_decision.mq_curmsgs;
    printf("\nMessages currently on the decision queue %ld\n",attr_decision.mq_curmsgs);
  
    while(count_decision){
      n=mq_receive(mqdes_decision,(int8_t*)&packet_recv_decision,4096,NULL);

      float val=0;
      val=atof(packet_recv_decision.data);

//      printf("\n\nDECISION LOD ID  %d  DATA  %f \n\n",packet_recv_decision.log_id,val);

      if(packet_recv_decision.log_id==1){
        if(val>1800){
          main_log_packet.log_level=ALERT;
          main_log_packet.log_id=DECISION_TASK;
          curtime=time(NULL);
          main_log_packet.timestamp=ctime(&curtime);
          strcpy(main_log_packet.data,packet_recv_decision.data);
          strcpy(main_log_packet.log_message,"ALERT TEMPERATURE EXCEEDED");
       
       }
       else if(val <500){
         main_log_packet.log_level=ALERT;
         main_log_packet.log_id=DECISION_TASK;
         curtime=time(NULL);
         main_log_packet.timestamp=ctime(&curtime);
         strcpy(main_log_packet.data,packet_recv_decision.data);
         strcpy(main_log_packet.log_message,"ALERT TEMPERATURE DROPPED");
        
       }
     }

     else if(packet_recv_decision.log_id==2){
       if(val>1800){
          main_log_packet.log_level=ALERT;
          main_log_packet.log_id=DECISION_TASK;
          curtime=time(NULL);
          main_log_packet.timestamp=ctime(&curtime);
          strcpy(main_log_packet.data,packet_recv_decision.data);
          strcpy(main_log_packet.log_message,"ALERT BRIGHT LIGHT");

       }
       else if(val <500){
         main_log_packet.log_level=ALERT;
         main_log_packet.log_id=DECISION_TASK;
         curtime=time(NULL);
         main_log_packet.timestamp=ctime(&curtime);
         strcpy(main_log_packet.data,packet_recv_decision.data);
         strcpy(main_log_packet.log_message,"ALERT DARK LIGHT");
       }
     }

     pthread_mutex_lock(&main_queue_lock);

     if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
        printf("\nError in mq_send main\n");
        exit(1);
     }

     pthread_mutex_unlock(&main_queue_lock);

      mq_getattr(mqdes_decision,&attr_decision);
      count_decision=attr_decision.mq_curmsgs;
    }

    ret=pthread_mutex_unlock(&decision_lock);
    if(ret){
      printf("\nMutex unlock decision failed in func4\n");
    }

    pthread_cond_broadcast(&heartbeat_decision);

    if(destroy_all==1){
      break;
    }
    usleep(10000);

  }

  printf("\nExiting Func 4\n");
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

