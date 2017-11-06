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
#include "i2c.h"

#define I2C_NUM 2

void timer_handler(int signum){
  cond_type=1;
  static int temp_counter=0;
  static int light_counter=0;
  temp_counter++;
  light_counter++;

  if(temp_counter==temp_timer_count){
    pthread_cond_broadcast(&timer_expire_temp);
    temp_counter=0;
  }

  if(light_counter==light_timer_count){
    pthread_cond_broadcast(&timer_expire_light);
    light_counter=0;
  }
}
                


void siginthandler( ){
  printf("\nIn SIGINT handler\n");
  log sig_log_packet;

  sig_log_packet.log_level=ERROR;
  sig_log_packet.log_id=MAIN_TASK;
  time_t curtime=time(NULL);
  sig_log_packet.timestamp=ctime(&curtime);
  strcpy(sig_log_packet.data,"-");

  strcpy(sig_log_packet.log_message,"EXIT GRACEFULLY");
 
 
  if(mq_send(mqdes_main,(const int8_t*)&sig_log_packet,sizeof(sig_log_packet),0)==-1){
    printf("\nError in mq_send main\n");
    exit(1);
  }
  
  usleep(10000);
  destroy_all=1;
  pthread_cond_broadcast(&timer_expire_temp);
  pthread_cond_broadcast(&timer_expire_light);


}


void sighandler1(int signum){
  printf("\nIn SIGHANDLER for USR1\n");
  printf("\nTemp task will  Query to light\n");

  request_log light_query;
  light_query.src_id=TEMP_TASK;
  light_query.dst_id=LIGHT_TASK;
  time_t curtime=time(NULL);
  light_query.timestamp=ctime(&curtime);
  light_query.command='d';
  light_query.delay=3;

  if(mq_send(mqdes_light_r,(const int8_t*)&light_query,sizeof(light_query),0)==-1){
    printf("\nError in mq_send light_query\n");
    exit(1);
  }

  cond_type_light=2;


}



void sighandler2(int signum){
  printf("\nIn SIGHANDLER for USR2\n");
  printf("\nLight thread will query temperature thread\n");

  request_log temp_query;
  temp_query.src_id=LIGHT_TASK;
  temp_query.dst_id=TEMP_TASK;
  time_t curtime=time(NULL);
  temp_query.timestamp=ctime(&curtime);
  temp_query.command='k';
  temp_query.delay=5;

  if(mq_send(mqdes_temp_r,(const int8_t*)&temp_query,sizeof(temp_query),0)==-1){
   printf("\nError in mq_send temp_query\n");
   exit(1);
}

   cond_type_temp=2;
}



int main(int argc,char* argv[]){
  temp_timer_count=1;
  light_timer_count=1;

  char file_name[100] = "log.txt";
  char path_name[100];
  char *path = getenv("PWD");
  strcpy(path_name,path);
   
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
    heartbeat_timer=heartbeat_init(5,4000000);

    asyncmain_to_light++;

    if(asyncmain_to_light==20){

      request_log light_query;
      light_query.src_id=MAIN_TASK;
      light_query.dst_id=LIGHT_TASK;
      curtime=time(NULL);
      light_query.timestamp=ctime(&curtime);
      light_query.command='d';
      light_query.delay=2;

      if(mq_send(mqdes_light_r,(const int8_t*)&light_query,sizeof(light_query),0)==-1){
        printf("\nError in mq_send light_query from main\n");
        exit(1);
      }

      cond_type_light=2;
    }

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
    }
    else{
      printf("\nLogger thread not working\n");
    
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
