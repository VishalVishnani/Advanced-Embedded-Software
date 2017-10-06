/**********************************************************
* Author: Vishal Vishnani
* Date: 10/5/2017
* File: thread_self.c
* Description: Source file for pthread_self example
***********************************************************/


//Includes
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>


//Function to be executed by threads
void* thread_fn(void* t){

  //check the thread id assigned to thread and display it
  printf("In thread_function, thread id = %ld\n",(long)pthread_self());
  pthread_exit(NULL);

}


int main(){

  int8_t rc;
  //pthread
  pthread_t thread;

  //create pthread to run thread_fn function
  rc=pthread_create(&thread,NULL,thread_fn,NULL);
  if(rc)
    printf("\nPthread create failed\n");

  //check and display the value assigned to thread
  printf("\nIn main,thread id assigned = %ld\n",(long)thread);

  //wait for thread to join
  rc=pthread_join(thread,NULL);
  if(rc)
    printf("\nPthread join failed\n");
  return 0;


}
