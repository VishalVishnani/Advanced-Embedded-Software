#include "thread.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>


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
      pthread_cancel(threads[1]);
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
