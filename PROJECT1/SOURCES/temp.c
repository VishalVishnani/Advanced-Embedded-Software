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
         pthread_cond_broadcast(&timer_expire_light);
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



