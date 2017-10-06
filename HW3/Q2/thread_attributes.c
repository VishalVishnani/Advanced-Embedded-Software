/****************************************************************
* Author: Vishal Vishnani
* Date: 10/5/2017
* File: thread_attribute.c
* Description: Source file for pthread setting attribute example
*****************************************************************/


//Includes
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

//Macros
#define N 2000
#define GSIZE 50000
#define NTHREADS 4

//Variable to set attribute 
pthread_attr_t attr;

//Function executed by threads
void* randomwork(void* threadid){
  uint32_t A[N][N];
  uint32_t i,j;
  long tid=(long)threadid;
  size_t newstacksize;
  int8_t concurrency;
  int8_t ret,policy,inherit;

  //Get stacksize
  ret=pthread_attr_getstacksize(&attr,&newstacksize);
  if(ret)
    printf("\npthread_attr_getstacksize failed in randomwork\n");

  printf("\nThread %ld Stacksize %li \n",tid, newstacksize);

  //get concurrency level
  concurrency=pthread_getconcurrency();

  printf("Thread %ld Concurrency Level %d \n",tid,concurrency);

  //get scheduling policy
  ret=pthread_attr_getschedpolicy(&attr,(int*)&policy);
  if(ret)
    printf("\npthread_attr_getschedpolicy failed in randomwork\n");


  if(policy==0)
    printf("Thread %ld scheduling policy SCHED_OTHER\n",tid);
  else if(policy==1)
    printf("Thread %ld scheduling policy SCHED_FIFO\n",tid);
  else
    printf("Thread %ld scheduling policy SCHED_RR\n",tid);


  //get default inheritance
  ret=pthread_attr_getinheritsched(&attr,(int*)&(inherit));
  if(ret)
    printf("\npthread_attr_getinheritsched failed in randomwork\n");


  if(inherit==0)
    printf("Thread %ld  inheritance inherit\n",tid);
  else
    printf("Thread %ld inheritance explicit\n\n",tid);



  
  //Random processing
  for(i=0;i<N;i++)
    for(j=0;j<N;j++)
      A[i][j]=(i+j);

}


int main()
{
  pthread_t threads[NTHREADS];
  size_t stacksize;
  int8_t ret;
  long k;
  void *status;
  int8_t concurrency_level;
  int8_t policy;
  int8_t level=2;
  int8_t inherit;

  //Initialize attribute
  ret=pthread_attr_init(&attr);
  if(ret)
    printf("\npthread_attr_init failed\n");


  //Get default stacksize
  ret=pthread_attr_getstacksize(&attr,&stacksize);
  if(ret)
    printf("\npthread_attr_getstacksize failed in main\n");
  

  printf("Default stack size is %li\n",stacksize);

  //change stacksize
  stacksize=sizeof(long int)*N*N+GSIZE;
  printf("Amount of stack needed per thread is %li\n",stacksize);


  //Set stacksize
  ret=pthread_attr_setstacksize(&attr,stacksize);
  if(ret)
    printf("\npthread_attr_setstacksize failed in main\n");


  //get default concurrency level
  //will be 0
  concurrency_level=pthread_getconcurrency();
  printf("\nDefault concurrency level is %d\n",concurrency_level);

  //set concurrency level to 2
  ret=pthread_setconcurrency(level);
  if(ret)
    printf("\npthread_attr_setconcurrency failed in main\n");



  //get default scheduling policy
  ret=pthread_attr_getschedpolicy(&attr,(int*)&policy);
  
  if(policy==0)
    printf("\nThe default scheduling policy is SCHED_OTHER\n");
  else if(policy==1)
    printf("\nThe default scheduling policy is SCHED_FIFO\n");
  else
    printf("\nThe default scheduling policy is SCHED_RR\n");


  //set scheduling policy
  ret=pthread_attr_setschedpolicy(&attr,SCHED_RR);
  if(ret)
    printf("\npthread_attr_setschedpolicy failed in main\n");



  //get default inheritance
  ret=pthread_attr_getinheritsched(&attr,(int*)&(inherit));
  if(ret)
    printf("\npthread_attr_getinheritsched failed in main\n");

  
  if(inherit==0)
    printf("\nThe default inheritance is inherit\n");
  else
    printf("\nThe default inheritance is explicit\n");

  //set inheritance value
  ret=pthread_attr_setinheritsched(&attr,PTHREAD_INHERIT_SCHED);
  if(ret)
    printf("\npthread_attr_setinheritsched failed in main\n");


  printf("\nCreating threads with stacksize %li and concurrency level 2 and scheduling policy SCHED_RR and inheritance to INHERIT\n\n",stacksize);


  for(k=0;k<NTHREADS;k++){

    //Create threads of given attributes
    ret=pthread_create(&threads[k],&attr,randomwork,(void*)k);
    if(ret){
      printf("error in creating thread %ld\n",k);
    }
  }

  printf("created %ld threads\n",k);


  for(k=0;k<NTHREADS;k++){

    //wait for threads to join
    ret=pthread_join(threads[k],&status);
    if(ret){
      printf("\nerror in joining thread %ld\n",k);
    }
  }

  printf("\nJoined %ld threads\n",k);

  pthread_exit(NULL);

}
