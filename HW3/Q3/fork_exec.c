/**********************************************************
* Author: Vishal Vishnani
* Date: 10/5/2017
* File: fork_exec.c
* Description: Source file for fork-exec
***********************************************************/



//Includes
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>


//Function to be executed by pthread
void* func1 (void * a){
  printf("\nIn pthread create\n");
}


int main(){
  pid_t child_pid;
  int status;
  int8_t rc;
  pthread_t thread;
  clock_t start_time,stop_time;
  int8_t ret;
    
  //start the timer for fork and exec
  start_time=clock();
 
  char* arg_list[]={"./EXEC",NULL};

  //fork
  child_pid=fork();

  //parent process
  if(child_pid!=0){
    printf("\nParent process\n");
  }

  //child process
  else{
    execvp("./EXEC",arg_list);
    fprintf(stderr, "an error occured\n");
    abort();
  }
  
  //wait for the child process
  wait(&status);

  //stop the timer for fork exec
  stop_time=clock();
  
  printf("\nTime taken in clock cycles for fork and exec: %ld\n",stop_time-start_time);


  //start the timer for pthread_create
  start_time=clock();
  

  //create pthread
  rc=pthread_create(&thread,NULL,func1,NULL);
  if(rc){
    printf("\nerror in pthread create\n");
  }

  //stop timer for create pthread
  stop_time=clock();

  //wait for threads to join
  rc=pthread_join(thread,NULL);
  if(rc)
    printf("\nerror in pthread join\n");
 
  printf("\nTime taken in clock cycles for pthread create: %ld\n",stop_time-start_time);
  
  return 0;
}

