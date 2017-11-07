/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : temp.c
* Description : Source file for temp
*               -func1()
******************************************************************************************/



#include "thread.h"
#include "logger.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include "timer.h"
#include "i2c.h"
#include "i2c_temp.h"



/******************************************************************************************
*
* func1() - Temperature thread which takes the temperature value from the sensor at
*           periodic intervals,stores it in Temperature Queue and also receives 
*           asynchronous requests from other threads in another queues and services 
*           that request. It also periodically updates main that it is working
********************************************************************************************/
void *func1(void* t){

  int8_t filename[20];
  int32_t file;
  snprintf(filename,19,"/dev/i2c-%d",I2C_NUM);
  file=setup_i2c(filename,(uint8_t)DEV_ADDR_TEMP);


  char logger_level[4][15]={"INFO","ERROR","SENSOR_VALUE","ALERT"};
  char task_no[5][15]={"MAIN_TASK","TEMP_TASK","LIGHT_TASK","LOGGER_TASK","DECISION_TASK"};

  log packet_temp;
  request_log req_temp;
  log packet_temp_async;

  int8_t ret=-1;

  time_t curtime=time(NULL);
  uint32_t count_req=0;
  check_alive=0;

  while(1){
    /*Used to signal main that temperature thread is working*/
    pthread_cond_broadcast(&heartbeat_temp);

    /*If variable is set to 1, exit while(1) loop*/
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
         pthread_cond_broadcast(&timer_expire_light);
    	 ret=pthread_mutex_unlock(&temp_w_lock);
    	 if(ret){
      		printf("\nError: mutex unlock failed in func1\n");
    	 }
 
	  continue;
    }


    /*Check if there is async query in temp query queue and service it first*/	
    if(cond_type_temp==2){
      printf("\n--------------------------ASYNC QUERY FOR TEMP----------------------------\n");
      mq_getattr(mqdes_temp_r,&attr_temp_r);
      count_req=attr_temp_r.mq_curmsgs;

      while(count_req){
        int n;
        n=mq_receive(mqdes_temp_r,(int8_t*)&req_temp,4096,NULL);
        if(n==0 || n<0){
          printf("\nNo message\n");
        }

        packet_temp_async.log_level=INFO;
        packet_temp_async.log_id=TEMP_TASK;
    
        int16_t temp_data=read_temperature(file,(uint8_t)TEMP_REG);
        
        float data;
        int8_t sign=1;

        /*For negative temperature*/
        if(temp_data > 0x800){
          temp_data=~temp_data;
          temp_data &=0x0FFF;
          temp_data=temp_data + 0x01;    
          sign=-1;
        }

        /*Conversion in degree celsius*/
        data=temp_data*(0.0625)*sign;
      

        /*If request is to get data in kelvin*/
        if(req_temp.command=='k'){
        data=data+273.15;
        sprintf(packet_temp_async.log_message,"TEMPERATURE DATA IN KELVIN QUERIED BY %s",task_no[req_temp.src_id]);
        packet_temp_async.timestamp=ctime(&curtime);
        sprintf(packet_temp_async.data,"%f",data);
      }

      /*If request is to get data in fahrenheit*/
      else if(req_temp.command=='f'){
        data=(data*1.8)+32;
        sprintf(packet_temp_async.log_message,"TEMPERATURE DATA IN FAHRENHEIT QUERIED BY %s",task_no[req_temp.src_id]);
        packet_temp_async.timestamp=ctime(&curtime);
        sprintf(packet_temp_async.data,"%f",data);
      }

      /*If request is to delay temperature timer*/
      else if(req_temp.command=='d'){
        sprintf(packet_temp_async.log_message,"TEMPERATURE DATA TIMER DELAYED BY %d BY %s",req_temp.delay,task_no[req_temp.src_id]);
        packet_temp_async.timestamp=ctime(&curtime);
        sprintf(packet_temp_async.data,"%f",data);
        temp_timer_count=req_temp.delay;
      }

      else{
        sprintf(packet_temp_async.log_message,"TEMPERATURE DATA IN CELSIUS QUERIED BY %s",task_no[req_temp.src_id]);
        packet_temp_async.timestamp=ctime(&curtime);
        sprintf(packet_temp_async.data,"%f",data);

       }

      if(mq_send(mqdes_temp_w,(const int8_t*)&packet_temp_async,sizeof(packet_temp_async),0)==-1){
        printf("\nError in mq_send temp\n");
        exit(1);
      }


        mq_getattr(mqdes_temp_r,&attr_temp_r);
        count_req=attr_temp_r.mq_curmsgs;
      }
      cond_type_temp=0;

    }

    else if(cond_type==1){

      strcpy(packet_temp.log_message,"TEMPERATURE DATA");
      packet_temp.timestamp=ctime(&curtime);

      if(ioctl(file, I2C_SLAVE, (uint8_t)DEV_ADDR_TEMP)<0){
        printf("\nERROR: Slave Address Resolution\n");
        exit(1);
       }

      /*Read temperature data value from sensor*/
      int16_t temp_data=read_temperature(file,(uint8_t)TEMP_REG);
      float data;
      int8_t sign=1;

      /*Neagtive conversion*/
      if(temp_data > 0x800){
        temp_data=~temp_data;
        temp_data &=0x0FFF;
        temp_data=temp_data + 0x01;
        sign=-1;
      }

      data=temp_data*(0.0625)*sign;


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

  close(file);
  printf("\nExiting Func 1\n");
  pthread_exit(NULL);
}



