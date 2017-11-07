/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : light.c
* Description : Source file for light threads
*               -func2()
******************************************************************************************/




#include "thread.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include "logger.h"
#include "timer.h"
#include "i2c.h"
#include "i2c_light.h"


/******************************************************************************************
*
* func2() - Light thread which takes the temperature value from the sensor at
*           periodic intervals,stores it in Light Queue and also receives 
*           asynchronous requests from other threads in another queue and services 
*           that requests. It also periodically updates main that it is working
********************************************************************************************/

void *func2(void* t){

  int8_t filename[20];
  int32_t file;
  snprintf(filename,19,"/dev/i2c-%d",I2C_NUM);
  file=setup_i2c(filename,(uint8_t)DEV_ADDR_LIGHT);

  char logger_level[4][15]={"INFO","ERROR","SENSOR_VALUE","ALERT"};
  char task_no[5][15]={"MAIN_TASK","TEMP_TASK","LIGHT_TASK","LOGGER_TASK","DECISION_TASK"};

  log packet_light;
  request_log req_light;
  log packet_light_async;
  int8_t ret=-1;

  time_t curtime=time(NULL);
  uint32_t count_req=0;

  while(1){
    /*Heartbeat used to signal main that its working*/
    pthread_cond_broadcast(&heartbeat_light);

  // check_alive++;

    /*Used to kill all threads and exit gracefully if one stops working*/
    if(check_alive==40){
      pthread_cancel(threads[1]);
    }
      

    /*Break this loop, if this variable is set and exit gracefully*/
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

    /*Check if there is async query for light task and service it first*/
    if(cond_type_light==2){
      printf("\n----------------ASYNC QUERY FOR LIGHT----------------------\n");
      mq_getattr(mqdes_light_r,&attr_light_r);
      count_req=attr_light_r.mq_curmsgs;

      while(count_req){

        int n;
        n=mq_receive(mqdes_light_r,(int8_t*)&req_light,4096,NULL);

        packet_light_async.log_level=INFO;
        packet_light_async.log_id=LIGHT_TASK;
 
        /*Read ADC light values from sensor*/
        light_write_read_control_register(file,(uint8_t)POWER_UP,1);
        if(ioctl(file, I2C_SLAVE, (uint8_t)DEV_ADDR_LIGHT)<0){
          printf("\nERROR: Slave Address Resolution\n");
          exit(1);
        }

        uint16_t sensor_value_light=light_read_sensor_data_value(file);

        float data= (float)sensor_value_light;

       /*If request is to delay light timer*/
       if(req_light.command=='d'){
        sprintf(packet_light_async.log_message,"LIGHT DATA TIMER CHANGED TO %d  BY %s",req_light.delay,task_no[req_light.src_id]);
        packet_light_async.timestamp=ctime(&curtime);
        sprintf(packet_light_async.data,"%f",data);
        light_timer_count=req_light.delay;

      }
      else{
        sprintf(packet_light_async.log_message,"LIGHT DATA QUERIED BY %s",task_no[req_light.src_id]);
        packet_light_async.timestamp=ctime(&curtime);
        sprintf(packet_light_async.data,"%f",data);


       }

       if(mq_send(mqdes_light_w,(const int8_t*)&packet_light_async,sizeof(packet_light_async),0)==-1){
        printf("\nError in mq_send light\n");
        exit(1);
      }


        mq_getattr(mqdes_light_r,&attr_light_r);
        count_req=attr_light_r.mq_curmsgs;
      }
      cond_type_light=0;
    }


    else if(cond_type==1){
      strcpy(packet_light.log_message,"LIGHT");
      packet_light.timestamp=ctime(&curtime);

      if(ioctl(file, I2C_SLAVE, (uint8_t)DEV_ADDR_LIGHT)<0){
          printf("\nERROR: Slave Address Resolution\n");
          exit(1);
      }

      /*Read light adc values from sensor*/
      light_write_read_control_register(file,(uint8_t)POWER_UP,1);
      uint16_t sensor_value_light=light_read_sensor_data_value(file);

      float data= (float)sensor_value_light;

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
 
  close(file);
  printf("\nExiting Func 2\n");
  pthread_exit(NULL);

}
