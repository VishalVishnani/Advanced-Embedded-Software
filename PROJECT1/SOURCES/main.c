#include <stdint.h>
#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "timer.h"
#include "queue.h"
#include "thread.h"




void timer_handler(int signum){
  cond_type=1;
  pthread_cond_broadcast(&timer_expire_temp);
  pthread_cond_broadcast(&timer_expire_light);
  
}
                


void siginthandler( ){
 // destroy_all=1;
  printf("\nIn SIGINT handler\n");
  log sig_log_packet;

  sig_log_packet.log_level=ERROR;
  sig_log_packet.log_id=MAIN_TASK;
  time_t curtime=time(NULL);
  sig_log_packet.timestamp=ctime(&curtime);
  strcpy(sig_log_packet.data,"-");

  strcpy(sig_log_packet.log_message,"EXIT GRACEFULLY");
  pthread_mutex_lock(&main_queue_lock);
 // pthread_cond_broadcast(&heartbeat_temp);
  //pthread_cond_broadcast(&heartbeat_light);
  //pthread_cond_broadcast(&heartbeat_logger);
 
  if(mq_send(mqdes_main,(const int8_t*)&sig_log_packet,sizeof(sig_log_packet),0)==-1){
    printf("\nError in mq_send main\n");
    exit(1);
  }
 // printf("\nMESSAGE SENT\n");
  pthread_mutex_unlock(&main_queue_lock);
  
  usleep(10000);
  destroy_all=1;
  pthread_cond_broadcast(&timer_expire_temp);
  pthread_cond_broadcast(&timer_expire_light);


}


void sighandler1(int signum){
  printf("\nIn SIGHANDLER for USR1\n");
  cond_type=2;
  request_log* temp_query=(request_log*)malloc(sizeof(temp_query));
  temp_query->src_id=0;
  temp_query->dst_id=1;
  time_t curtime=time(NULL);
  temp_query->timestamp=ctime(&curtime);
  char buffer[]="GET_TEMP";
  temp_query->message=buffer;
 
  pthread_mutex_lock(&temp_w_lock);
  
  if(mq_send(mqdes_temp_r,(const int8_t*)&temp_query,sizeof(temp_query),0)==-1){
    printf("\nError in mq_send temp_query\n");
    exit(1);
  }
 
  pthread_mutex_unlock(&temp_w_lock);

  //pthread_cond_broadcast(&timer_expire);

}



void sighandler2(int signum){
  printf("\nIn SIGHANDLER for USR2\n");
  cond_type=3;

  request_log* light_query=(request_log*)malloc(sizeof(light_query));
  light_query->src_id=0;
  light_query->dst_id=2;
  time_t curtime=time(NULL);
  light_query->timestamp=ctime(&curtime);
  char buffer[]="GET_LIGHT";
  light_query->message=buffer;

  pthread_mutex_lock(&light_w_lock);

  if(mq_send(mqdes_light_r,(const int8_t*)&light_query,sizeof(light_query),0)==-1){
    printf("\nError in mq_send light_query\n");
    exit(1);
  }

  pthread_mutex_unlock(&light_w_lock);

  //pthread_cond_broadcast(&timer_expire);

}



int main(int argc,char* argv[]){
  char file_name[100] = "log.txt";
  char path_name[100];
  char *path = getenv("PWD");
  strcpy(path_name,path);

  char c;
  while((c = getopt(argc,argv,"f:p:")) != -1)
  {
    switch(c)
    {
      case 'f':
        memset(file_name,0,100);
        strcpy(file_name,optarg);
        break;

      case 'p':
        memset(path_name,0,100);
        strcpy(path_name,optarg);
        break;

      case '?':
        printf("\nWrong arguments given!!!\n");
        exit(1);

      default:
        exit(1);
    }

  }

  strcat(path_name,"/");
  strcat(file_open,path_name);
  strcat(file_open,file_name);

  //if file already exists remove
  remove(file_open);
  

  initialize_queues();

  int8_t ret=1;

  pthread_initialize();

  log main_log_packet;
  main_log_packet.log_level=INFO;
  main_log_packet.log_id=MAIN_TASK;
  time_t curtime=time(NULL);
  main_log_packet.timestamp=ctime(&curtime);

  strcpy(main_log_packet.log_message,"THREADS SPAWNED");
  strcpy(main_log_packet.data,"-");

  pthread_mutex_lock(&main_queue_lock);
 
  if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
    printf("\nError in mq_send main\n");
    exit(1);
  }
    
  pthread_mutex_unlock(&main_queue_lock);


  signal(SIGINT,siginthandler);  
  signal(SIGUSR1,sighandler1);
  signal(SIGUSR2,sighandler2);
  
  timer_init();
  struct timespec heartbeat_timer;

  while(1){
    heartbeat_timer=heartbeat_init(3,4000000);

    if(destroy_all==1){
      pthread_cond_broadcast(&timer_expire_temp);
      pthread_cond_broadcast(&timer_expire_light);
      printf("\nDestroy main\n");
      break;
    }

    
    ret=pthread_mutex_lock(&heart_beat_temp_lock);
    if(ret){
      printf("\nError: heartbeat mutex lock failed in main\n");
    }
    int8_t ret1=pthread_cond_timedwait(&heartbeat_temp,&heart_beat_temp_lock,&heartbeat_timer);

    ret=pthread_mutex_unlock(&heart_beat_temp_lock);
    if(ret){
      printf("\nError:heartbeat mutex lock failed in main\n");
    }

    if(ret1==0){
      printf("\nTemp thread is working\n");
    }
    else{
      printf("\nTemp thread not working\n");
      main_log_packet.log_level=ERROR;
      main_log_packet.log_id=MAIN_TASK;
      curtime=time(NULL);
      main_log_packet.timestamp=ctime(&curtime);
      strcpy(main_log_packet.log_message,"TEMPERATURE THREAD STUCK");
      strcpy(main_log_packet.data,"-");

      pthread_mutex_lock(&main_queue_lock);

      if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
        printf("\nError in mq_send main\n");
        exit(1);
      }

      pthread_mutex_unlock(&main_queue_lock);


      siginthandler();
      continue;
    }



    ret=pthread_mutex_lock(&heart_beat_light_lock);
    if(ret){
      printf("\nError: heartbeat mutex lock failed in main\n");
    }
    ret1=pthread_cond_timedwait(&heartbeat_light,&heart_beat_light_lock,&heartbeat_timer);

    ret=pthread_mutex_unlock(&heart_beat_light_lock);
    if(ret){
      printf("\nError: heartbeat mutex lock failed in main\n");
    }

    if(ret1==0){
      printf("\nLight thread is working\n");
    }
    else{
      printf("\nLight thread not working\n");
      
      main_log_packet.log_level=ERROR;
      main_log_packet.log_id=MAIN_TASK;
      curtime=time(NULL);
      main_log_packet.timestamp=ctime(&curtime);
 
      strcpy(main_log_packet.log_message,"LIGHT THREAD STUCK");
      strcpy(main_log_packet.data,"-");

      pthread_mutex_lock(&main_queue_lock);

      if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
        printf("\nError in mq_send main\n");
        exit(1);
      }

      pthread_mutex_unlock(&main_queue_lock);



      siginthandler();
      continue;
    }


    ret=pthread_mutex_lock(&heart_beat_logger_lock);
    if(ret){
      printf("\nError: heartbeat mutex lock failed in main\n");
    }
    ret1=pthread_cond_timedwait(&heartbeat_logger,&heart_beat_logger_lock,&heartbeat_timer);

    ret=pthread_mutex_unlock(&heart_beat_logger_lock);
    if(ret){
      printf("\nError: heartbeat mutex lock failed in main\n");
    }

    if(ret1==0){
      printf("\nlogger thread is working\n");
    }
    else{
      printf("\nLogger thread not working\n");

      main_log_packet.log_level=ERROR;
      main_log_packet.log_id=MAIN_TASK;
      curtime=time(NULL);
      main_log_packet.timestamp=ctime(&curtime);
      strcpy(main_log_packet.data,"-");
      strcpy(main_log_packet.log_message,"LOGGER THREAD STUCK");
      pthread_mutex_lock(&main_queue_lock);

      if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
        printf("\nError in mq_send main\n");
        exit(1);
      }

      pthread_mutex_unlock(&main_queue_lock);


      siginthandler();
      continue;
    }


    ret=pthread_mutex_lock(&heart_beat_decision_lock);
    if(ret){
      printf("\nError: heartbeat mutex lock failed in main\n");
    }
    ret1=pthread_cond_timedwait(&heartbeat_decision,&heart_beat_decision_lock,&heartbeat_timer);

    ret=pthread_mutex_unlock(&heart_beat_decision_lock);
    if(ret){
      printf("\nError: heartbeat mutex lock failed in main\n");
    }

    if(ret1==0){
      printf("\nDecision thread is working\n");
    }
    else{
      printf("\nDecision thread not working\n");

      main_log_packet.log_level=ERROR;
      main_log_packet.log_id=MAIN_TASK;
      curtime=time(NULL);
      main_log_packet.timestamp=ctime(&curtime);
      strcpy(main_log_packet.data,"-");
      strcpy(main_log_packet.log_message,"DECISION THREAD STUCK");
      pthread_mutex_lock(&main_queue_lock);

      if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
        printf("\nError in mq_send main\n");
        exit(1);
      }

      pthread_mutex_unlock(&main_queue_lock);


      siginthandler();
      continue;
    }


    usleep(100000);        
  
 
  }

  int8_t i=0;
  ret=-1;

  for(i=0;i<4;i++){
    ret=pthread_join(threads[i],NULL);
    if(ret){
      printf("\nError in pthread join %d\n",i);
    }
  }


  printf("\nDestroying mutex and cond\n");
  ret=pthread_mutex_destroy(&temp_w_lock);
  if(ret){
    printf("\nError in temp mutex destroy\n");
  }

  ret=pthread_mutex_destroy(&light_w_lock);
  if(ret){
    printf("\nError in light mutex destroy\n");
  }

  ret=pthread_mutex_destroy(&heart_beat_temp_lock);
  if(ret){
    printf("\nError in heartbeat mutex destroy\n");
  }

  ret=pthread_mutex_destroy(&heart_beat_light_lock);
  if(ret){
    printf("\nError in heartbeat mutex destroy\n");
  }

  ret=pthread_mutex_destroy(&heart_beat_logger_lock);
  if(ret){
    printf("\nError in heartbeat mutex destroy\n");
  }

 
  ret=pthread_mutex_destroy(&main_queue_lock);
  if(ret){
    printf("\nError in main_queue mutex destroy\n");
  }

  ret=pthread_mutex_destroy(&decision_lock);
  if(ret){
    printf("\nError in decision mutex destroy\n");
  }

  ret=pthread_mutex_destroy(&heart_beat_decision_lock);
  if(ret){
    printf("\nError in heartbeat decision mutex destroy\n");
  }


  ret=pthread_cond_destroy(&timer_expire_temp);
  if(ret){
    printf("\nError in timer cond destroy\n");
  }

  ret=pthread_cond_destroy(&timer_expire_light);
  if(ret){
    printf("\nError in timer cond destroy\n");
  }
  
  ret=pthread_cond_destroy(&heartbeat_temp);
  if(ret){
    printf("\nError in heartbeat temp cond destroy\n");
  }
  
  ret=pthread_cond_destroy(&heartbeat_light);
  if(ret){
    printf("\nError in heartbeat light cond destroy\n");
  }

  ret=pthread_cond_destroy(&heartbeat_logger);
  if(ret){
    printf("\nError in heartbeat logger cond destroy\n");
  }


  ret=pthread_cond_destroy(&heartbeat_decision);
  if(ret){
    printf("\nError in heartbeat decision cond destroy\n");
  }

  mq_close(mqdes_temp_w);
  mq_close(mqdes_light_w);

  mq_close(mqdes_temp_r);
  mq_close(mqdes_light_r);

  mq_close(mqdes_main);
  mq_close(mqdes_decision);

  printf("\nExiting main\n");
  pthread_exit(NULL);

 
  return 0;

}
