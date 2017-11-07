/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : decision.c
* Description : Source file for decison thread
*               -func4()
*****************************************************************************************/


/*INCLUDES*/
#include "thread.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include "logger.h"
#include "led.h"


#define TEMP_LOW (0)
#define TEMP_HIGH (50)
#define LIGHT_LOW (500)
#define LIGHT_HIGH (20000)

#define TEMP_ID (1)
#define LIGHT_ID (2)

/******************************************************************************************
*
* func4() - Decisiom thread which reads from queue and takes decision based on it (ALERTS). 
*           It also periodically updates main that it is working
********************************************************************************************/
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
    /*Used to signal main that decision thread is working*/
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
  
    /*Reads messages from decision queue and take decision and log it*/
    while(count_decision){
      n=mq_receive(mqdes_decision,(int8_t*)&packet_recv_decision,4096,NULL);

      float val=0;
      val=atof(packet_recv_decision.data);

    
      /*If temperature value greater than 50, send ALERT to logger*/
      if(packet_recv_decision.log_id==TEMP_ID){
        if(val>TEMP_HIGH){
          main_log_packet.log_level=ALERT;
          main_log_packet.log_id=DECISION_TASK;
          curtime=time(NULL);
          main_log_packet.timestamp=ctime(&curtime);
          strcpy(main_log_packet.data,packet_recv_decision.data);
          strcpy(main_log_packet.log_message,"ALERT TEMPERATURE EXCEEDED");
          pthread_mutex_lock(&main_queue_lock);

          if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
            printf("\nError in mq_send main\n");
            exit(1);
          }

          pthread_mutex_unlock(&main_queue_lock);

       
       }
      
       /*If tempertaure value less than 0, send alert to logger*/
       else if(val <TEMP_LOW){
         main_log_packet.log_level=ALERT;
         main_log_packet.log_id=DECISION_TASK;
         curtime=time(NULL);
         main_log_packet.timestamp=ctime(&curtime);
         strcpy(main_log_packet.data,packet_recv_decision.data);
         strcpy(main_log_packet.log_message,"ALERT TEMPERATURE DROPPED");

         pthread_mutex_lock(&main_queue_lock);

         if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
           printf("\nError in mq_send main\n");
           exit(1);
         }

         pthread_mutex_unlock(&main_queue_lock);

        
         }
     }

     /*If light sensor ADC value greater than 20000, send alert to logger and turn on led*/
     else if(packet_recv_decision.log_id==LIGHT_ID){
       if(val>LIGHT_HIGH){
          main_log_packet.log_level=ALERT;
          main_log_packet.log_id=DECISION_TASK;
          curtime=time(NULL);
          main_log_packet.timestamp=ctime(&curtime);
          strcpy(main_log_packet.data,packet_recv_decision.data);
          strcpy(main_log_packet.log_message,"ALERT BRIGHT LIGHT");
          pthread_mutex_lock(&main_queue_lock);

          if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
            printf("\nError in mq_send main\n");
            exit(1);
          }

          pthread_mutex_unlock(&main_queue_lock);

          led_on();

       }

       /*If light sensor ADC value less than 500, send alert to logger and turn off leds*/
       else if(val <LIGHT_LOW){
         main_log_packet.log_level=ALERT;
         main_log_packet.log_id=DECISION_TASK;
         curtime=time(NULL);
         main_log_packet.timestamp=ctime(&curtime);
         strcpy(main_log_packet.data,packet_recv_decision.data);
         strcpy(main_log_packet.log_message,"ALERT DARK LIGHT");
         pthread_mutex_lock(&main_queue_lock);

         if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
           printf("\nError in mq_send main\n");
           exit(1);
         }

         pthread_mutex_unlock(&main_queue_lock);

         led_off();
       }
     }

      mq_getattr(mqdes_decision,&attr_decision);
      count_decision=attr_decision.mq_curmsgs;
    }

    ret=pthread_mutex_unlock(&decision_lock);
    if(ret){
      printf("\nMutex unlock decision failed in func4\n");
    }

    pthread_cond_broadcast(&heartbeat_decision);

    /*Exit while(1) loop, when this variable is 1*/
    if(destroy_all==1){
      break;
    }
    usleep(10000);

  }

  printf("\nExiting Func 4\n");
  pthread_exit(NULL);
}

