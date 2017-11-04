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
  pthread_cond_broadcast(&timer_expire);
}
                


void siginthandler(int signum){
  printf("\nIn SIGINT handler\n");
  destroy_all=1;
  pthread_cond_broadcast(&timer_expire);

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

  pthread_cond_broadcast(&timer_expire);

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

  pthread_cond_broadcast(&timer_expire);

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
  timer_init();

  int8_t ret=1;

  pthread_initialize();

  log* main_log_packet=(log*)malloc(sizeof(log));
  main_log_packet->log_level=INFO;
  main_log_packet->log_id=MAIN_TASK;
  time_t curtime=time(NULL);
  main_log_packet->timestamp=ctime(&curtime);
  char buffer[]="THREADS SPAWNED";
  main_log_packet->log_message=buffer;

  pthread_mutex_lock(&main_queue_lock);
 
  if(mq_send(mqdes_main,(const int8_t*)&main_log_packet,sizeof(main_log_packet),0)==-1){
    printf("\nError in mq_send main\n");
    exit(1);
  }
    
  pthread_mutex_unlock(&main_queue_lock);

  signal(SIGINT,siginthandler);  
  signal(SIGUSR1,sighandler1);
  signal(SIGUSR2,sighandler2);
  
  while(1){
    if(destroy_all==1){
      break;
    }
  }
//  sleep(5);
  int8_t i=0;
  ret=-1;

  for(i=0;i<3;i++){
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

    
  ret=pthread_mutex_destroy(&main_queue_lock);
  if(ret){
    printf("\nError in temp mutex destroy\n");
  }


  ret=pthread_cond_destroy(&timer_expire);
  if(ret){
    printf("\nError in timer cond destroy\n");
  }

  
  mq_close(mqdes_temp_w);
  mq_close(mqdes_light_w);

  mq_close(mqdes_temp_r);
  mq_close(mqdes_light_r);

  mq_close(mqdes_main);

  printf("\nExiting main\n");
  pthread_exit(NULL);

 
  return 0;

}
