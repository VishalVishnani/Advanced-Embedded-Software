/*************************************************************
* Author: Vishal Vishnani
* Date: 10/5/2017
* File: trylock.c
* Description: Source file for pthread mutex trylock example
**************************************************************/


//Includes
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

//Macros
#define NTHREADS 3
#define COUNT_LIMIT 1000000

//Global variable to be modified by threads
uint32_t count=0;

//mutex
pthread_mutex_t count_mutex;


//Function to be executed by threads
void* write_count(void *t){
  long i;
  int8_t ret;

  for(i=0;i<COUNT_LIMIT;i++){

    //use mutex trylock as we update global variable shared among threads
    ret=pthread_mutex_trylock(&count_mutex);
    if(ret)
      printf("\npthread_mutex_trylock failed\n");

    count++;
    
    //unlock mutex
    ret=pthread_mutex_unlock(&count_mutex);
    if(ret)
    printf("\npthread_mutex_unlock failed\n");

  }
    
  pthread_exit(NULL);
}

int main()
{
  uint8_t i;
  int8_t ret;

  //pthreads
  pthread_t threads[NTHREADS];
  pthread_attr_t attr;
 
  //Initialize mutex
  ret=pthread_mutex_init(&count_mutex,NULL);
  if(ret)
    printf("\npthread_mutex_init failed\n");


  //Initialize attribute
  ret=pthread_attr_init(&attr);
  if(ret)
    printf("\npthread_attr_init failed\n");


  //Create pthreads to execute write count function
  for(i=0;i<NTHREADS;i++){
    ret=pthread_create(&threads[i],&attr,write_count,NULL);
    if(ret)
      printf("\nError in pthread create %d\n",i);

  }

  
  //wait for threads to join
  for(i=0;i<NTHREADS;i++){
    ret=pthread_join(threads[i],NULL);
    if(ret)
      printf("\npthread join %d failed\n",i);

  }
  printf("Threads joined\n");

  //final count value
  printf("\nCount=%d\n",count);

  //destroy attribute and mutex
  ret=pthread_attr_destroy(&attr);
  if(ret)
    printf("\npthread_attr_destroy failed\n");

  ret=pthread_mutex_destroy(&count_mutex);
  if(ret)
    printf("\npthread_mutex_destroy failed\n");

  pthread_exit(NULL);

}

