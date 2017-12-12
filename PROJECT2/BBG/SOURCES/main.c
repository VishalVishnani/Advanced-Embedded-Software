#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include "threads.h"
#include "queue.h"
#include "log.h"


void siginthandler(){
  printf("\nIn SIGINT handler\n");
  int8_t ret=0;

  log_packet packet;
  time_t curtime;
  memset(&packet,0,sizeof(packet));
  packet.level=ERROR;
  packet.task_ID=MAIN_TASK;
  strcpy(packet.log_message,"EXIT GRACEFULLY");
  strcpy(packet.data,"0");
  curtime=time(NULL);
  packet.current_time=ctime(&curtime);


  ret=pthread_mutex_trylock(&log_mutex);
  if(ret==0){

    if(mq_send(mqdes_logger,(const int8_t*)&packet,sizeof(packet),0)==-1){
      printf("\nError in mq_send logger in sig handler\n");
      exit(1);
    }

    ret=pthread_mutex_unlock(&log_mutex);
    if(ret){
      printf("\nError: log mutex unlock failed in sighandler\n");
    }
  }
  printf("\nEnd of sigint handler\n");
  exit_flag=1;
}


struct timespec heartbeat_init(uint32_t sec_value,uint32_t nsec_value){
  struct timespec t1;
  struct timeval t2;

  gettimeofday(&t2,NULL);
  
  t1.tv_sec=t2.tv_sec;
  t1.tv_nsec=(t2.tv_usec)*1000;
  t1.tv_sec+=sec_value;
  t1.tv_nsec+=nsec_value;

  return t1;

}



int main(int argc,char* argv[]){
  
  /*Used to take name of file from commandline to log*/
  char file_name[100] = "log.txt";
  char path_name[100];
  char *path = getenv("PWD");
  strcpy(path_name,path);
  int8_t ret=0;   
  int8_t ret1=0;
  log_packet main_packet;
  time_t curtime;
  struct timespec heartbeat_timer;
  int count_kill=0;

  if (argc == 3)
  { 
   strcpy(file_name,argv[1]);
   strcpy(path_name,argv[2]);
	
  }
  else if(argc == 2)
  { 
   strcpy(file_name,argv[1]);
  }
  else if(argc > 3)
  {
     printf("\nTOO MANY ARGUMENTS\n");
  }
  
  strcat(path_name,"/");
  strcat(file_open,path_name);
  strcat(file_open,file_name);

  //if file already exists remove
  remove(file_open);

  

  signal(SIGINT,siginthandler);
  init_queues();
  pthread_initialize();
  

  while(1){
    memset(&main_packet,0,sizeof(main_packet));

    heartbeat_timer=heartbeat_init(3,400000);

    count_kill++;

    if(count_kill==10){
      pthread_cancel(threads[2]);
    }


    if(exit_flag==1){
      break;
    }


    ret=pthread_mutex_lock(&heartbeat_mutex);
    if(ret){
      printf("\nHeartbeat mutex lock failed in main\n");
    }

    ret1=pthread_cond_timedwait(&hb_socket,&heartbeat_mutex,&heartbeat_timer);
    
    ret=pthread_mutex_unlock(&heartbeat_mutex);
    if(ret){
      printf("\nHeartbeat mutex unlock failed in main\n");
    }


    if(ret1!=0){
      printf("\nSocket thread not working\n");
     
      main_packet.level=ERROR;
      main_packet.task_ID=MAIN_TASK;
      strcpy(main_packet.log_message,"SOCKET THREAD NOT WORKING");
      strcpy(main_packet.data,"0");
      curtime=time(NULL);
      main_packet.current_time=ctime(&curtime);


      ret=pthread_mutex_lock(&log_mutex);
      if(ret){
        printf("\nError: log mutex lock failed in main\n");
      }

      if(mq_send(mqdes_logger,(const int8_t*)&main_packet,sizeof(main_packet),0)==-1){
        printf("\nError in mq_send logger in main\n");
        exit(1);
      }

      ret=pthread_mutex_unlock(&log_mutex);
      if(ret){
        printf("\nError: log mutex unlock failed in main\n");
      }
      
 

      siginthandler();
      continue;
    }




    ret=pthread_mutex_lock(&heartbeat_mutex);
    if(ret){
      printf("\nHeartbeat mutex lock failed in main\n");
    }

    ret1=pthread_cond_timedwait(&hb_decision,&heartbeat_mutex,&heartbeat_timer);

    ret=pthread_mutex_unlock(&heartbeat_mutex);
    if(ret){
      printf("\nHeartbeat mutex unlock failed in main\n");
    }


    if(ret1!=0){
      printf("\ndecision thread not working\n");
  
      main_packet.level=ERROR;
      main_packet.task_ID=MAIN_TASK;
      strcpy(main_packet.log_message,"DECISION THREAD NOT WORKING");
      strcpy(main_packet.data,"0");
      curtime=time(NULL);
      main_packet.current_time=ctime(&curtime);


      ret=pthread_mutex_lock(&log_mutex);
      if(ret){
        printf("\nError: log mutex lock failed in main\n");
      }

      if(mq_send(mqdes_logger,(const int8_t*)&main_packet,sizeof(main_packet),0)==-1){
        printf("\nError in mq_send logger in main\n");
        exit(1);
      }

      ret=pthread_mutex_unlock(&log_mutex);
      if(ret){
        printf("\nError: log mutex unlock failed in main\n");
      }



      siginthandler();
      continue;
    }

  
  
    
    ret=pthread_mutex_lock(&heartbeat_mutex);
    if(ret){
      printf("\nHeartbeat mutex lock failed in main\n");
    }

    ret1=pthread_cond_timedwait(&hb_logger,&heartbeat_mutex,&heartbeat_timer);

    ret=pthread_mutex_unlock(&heartbeat_mutex);
    if(ret){
      printf("\nHeartbeat mutex unlock failed in main\n");
    }


    if(ret1!=0){
      printf("\nLogger thread not working\n");
      siginthandler();
      continue;
    }
        
    
  }

  int i=0;
  for(i=0;i<3;i++){
    ret=pthread_join(threads[i],NULL);
    if(ret){
      printf("\nError in pthread join %d\n",i);
    }
  }

  cleanup_threads();

  cleanup_queues();

  printf("\nExiting main\n");

  pthread_exit(NULL);

  return 0;
}

