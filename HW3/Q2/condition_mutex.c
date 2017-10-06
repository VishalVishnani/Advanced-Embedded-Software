/********************************************************************
* Author: Vishal Vishnani
* Date: 10/5/2017
* File: condition_mutex.c
* Description: Source file for pthread condition signal and mutexes
*********************************************************************/


//Includes
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

//Macros
#define NTHREADS 2
#define COUNT_LIMIT 20


//Global variable to be shared among threads
uint8_t count=0;


//mutex and condition variables
pthread_mutex_t count_mutex;
pthread_cond_t count_condition;


//Function to be executed by child thread1
void * read_count(void *t){
 
  int8_t ret;

  //get the threadid
  long tid=(long)t;
  printf("Starting read count : thread %ld\n",tid);

  //acquire the mutex as we will read the count variable and then update it
  ret=pthread_mutex_lock(&count_mutex);
  if(ret)
    printf("\npthread_mutex_lock failed in read count\n");

  
  //wait for the count_condition signal
  ret=pthread_cond_wait(&count_condition,&count_mutex);
  if(ret)
    printf("\npthread_cond_wait failed in read count\n");

  printf("read count : thread %ld condition signal received\n",tid);

  //Update the count value after the signal is received
  count+=100;
  printf("read count : thread %ld count now %d\n",tid,count);

  //Unlock the mutex
  ret=pthread_mutex_unlock(&count_mutex);
  if(ret)
    printf("\npthread_mutex_unlock failed in read count\n");

  pthread_exit(NULL);

}


//Function to be executed by child thread2
void* write_count(void *t){
  uint8_t i;
  int8_t ret;
  //get the threadid
  long tid=(long)t;
  
  //Increment count value upto COUNT_LIMIT
  for(i=0;i<COUNT_LIMIT;i++){
    ret=pthread_mutex_lock(&count_mutex);
    if(ret)
      printf("\npthread_mutex_lock failed in write count\n");

    count++;
    
    //If count limit is reached, signal the count_condition signal on which the child thread1 is waiting
    if(count==COUNT_LIMIT){
      ret=pthread_cond_signal(&count_condition);
      if(ret)
        printf("\npthread_cond_signal failed in write count\n");

      printf("write count: thread %ld , count = %d , threshold reached.\n",tid,count);
    }
    
    printf("write count: thread %ld , count = %d, unlocking mutex\n",tid,count);
   
    //Unlock the mutex
    ret=pthread_mutex_unlock(&count_mutex);
    if(ret)
      printf("\npthread_mutex_unlock failed in write count\n");

    sleep(1);
  }
  pthread_exit(NULL);
}

int main()
{
  uint8_t i;
  long t1=1,t2=2;
  int8_t ret;

  //pthreads
  pthread_t threads[NTHREADS];
  pthread_attr_t attr;
 
  //pthread initialize mutex and condition variables
  ret=pthread_mutex_init(&count_mutex,NULL);
  if(ret)
    printf("\npthread_mutex_init failed\n");

  ret=pthread_cond_init(&count_condition,NULL);
  if(ret)
    printf("\npthread_cond_init failed\n");

  
  //pthread initialize attribute
  ret=pthread_attr_init(&attr);
  if(ret)
    printf("\npthread_attr_init failed\n");


  //create pthreads to execute read and write count functions
  ret=pthread_create(&threads[0],&attr,read_count,(void*)t1);
  if(ret)
    printf("\npthread_create 1 failed\n");

  ret=pthread_create(&threads[1],&attr,write_count,(void*)t2);
  if(ret)
    printf("\npthread_create 2 failed\n");


  //wait for threads to join
  for(i=0;i<NTHREADS;i++){
    ret=pthread_join(threads[i],NULL);
    if(ret)
      printf("\npthread_join i failed\n");


  }
  printf("Threads joined\n");

  //Destroy attribute,mutex and condition variables
  ret=pthread_attr_destroy(&attr);
  if(ret)
    printf("\npthread_attr_destroy failed\n");

  ret=pthread_mutex_destroy(&count_mutex);
  if(ret)
    printf("\npthread_mutex_destroy failed\n");

  ret=pthread_cond_destroy(&count_condition);
  if(ret)
    printf("\npthread_cond_destroy failed\n");

  pthread_exit(NULL);

}

