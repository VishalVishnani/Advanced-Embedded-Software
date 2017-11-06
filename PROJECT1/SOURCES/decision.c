#include "thread.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include "logger.h"


void *func4(void* t){
  char logger_level[4][15]={"INFO","ERROR","SENSOR_VALUE","ALERT"};
  char task_no[5][15]={"MAIN_TASK","TEMP_TASK","LIGHT_TASK","LOGGER_TASK","DECISION_TASK"};

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
  
    while(count_decision){
      n=mq_receive(mqdes_decision,(int8_t*)&packet_recv_decision,4096,NULL);

      float val=0;
      val=atof(packet_recv_decision.data);


      if(packet_recv_decision.log_id==1){
        if(val>50){
          main_log_packet.log_level=ALERT;
          main_log_packet.log_id=DECISION_TASK;
          curtime=time(NULL);
          main_log_packet.timestamp=ctime(&curtime);
          strcpy(main_log_packet.data,packet_recv_decision.data);
          strcpy(main_log_packet.log_message,"ALERT TEMPERATURE EXCEEDED");
       
       }
       else if(val <0){
         main_log_packet.log_level=ALERT;
         main_log_packet.log_id=DECISION_TASK;
         curtime=time(NULL);
         main_log_packet.timestamp=ctime(&curtime);
         strcpy(main_log_packet.data,packet_recv_decision.data);
         strcpy(main_log_packet.log_message,"ALERT TEMPERATURE DROPPED");
        
       }
     }

     else if(packet_recv_decision.log_id==2){
       if(val>5000){
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

