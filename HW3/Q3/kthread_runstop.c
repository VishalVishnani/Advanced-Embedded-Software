/**********************************************************
* Author: Vishal Vishnani
* Date: 9/6/2017
* File: kthread_runstop.c
* Description: Source file for profiling kthread create
***********************************************************/


//Includes
#include<linux/module.h>
#include<linux/init.h>
#include<linux/kthread.h>
#include<linux/kernel.h>
#include<linux/jiffies.h>
#include<linux/timer.h>
#include<linux/timex.h>
#include<linux/time.h>
#include<linux/ktime.h>

MODULE_LICENSE("GPL");

//kernel thread
static struct task_struct *thread_st;

//function to be executed by kthread
static int thread_fn(void* unused){
  printk(KERN_ALERT "Thread running\n");
  
  do_exit(0);
  return 0;
}

//Insert function
int init_thread(void){

  struct timeval t0,t1;
  long temp;
  long start;
  long end;

  printk(KERN_ALERT "Creating thread\n");

  //start time for jiffies
  start=jiffies;

  //start time for usec timer
  do_gettimeofday(&t0);  
  
  //create kthread
  thread_st=kthread_create(thread_fn,NULL,"mythread");

  if(thread_st){
    printk(KERN_ALERT "Thread created successfully\n");
  }
  else
    printk(KERN_ALERT "Thread creation failed\n");

  //stop time for usec timer
  do_gettimeofday(&t1);

  //stop time for jiffies
  end=jiffies;

  //calculate time diff in usecs
  temp=t1.tv_usec-t0.tv_usec;
  

  printk(KERN_ALERT "Thread creation time in usecs: %ld\n",temp);
  printk(KERN_ALERT "Thread creation cycles in jiffies: %ld\n",end-start);


  return 0;
}


//Remove module
void cleanup_thread(void){
  printk(KERN_ALERT "Cleaning up\n");
  
  if(thread_st){
    kthread_stop(thread_st);
    thread_st=NULL;
    printk(KERN_ALERT "Thread Stopped\n");
  }

}

MODULE_LICENSE("GPL");

module_init(init_thread);
module_exit(cleanup_thread);
