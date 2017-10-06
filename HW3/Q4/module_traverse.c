/****************************************************************************************
* Author: Vishal Vishnani
* Date: 10/5/2017
* File: module_traverse.c
* Description: Source file for traversing all the processes in the kernel linkedlist
****************************************************************************************/


//Includes
#include<linux/sched.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/list.h>


MODULE_LICENSE("GPL");

//Insert function
int module_start(void)
{
  struct task_struct *task;
  struct list_head* traverse_ptr;
  int16_t num_child=0;
  printk(KERN_INFO "Loading task module...\n");

  printk(KERN_INFO "Traverse backing from current process to current\n");

  //Traverse the parent of each task
  for(task=current;task!=&init_task;task=task->parent) {
    num_child=0;
  
    //for each task count the no. of children
    list_for_each(traverse_ptr,&(task->children)){
      num_child++;      
    }
    printk("%s [%d] state = %ld  children= %d\n",task->comm,task->pid,task->state,num_child);
  }


  printk(KERN_INFO "Listing all the current process\n");
  for_each_process(task){
     num_child=0;
  
    //for each task count the no. of children
    list_for_each(traverse_ptr,&(task->children)){
      num_child++;      
    }
    printk("%s [%d] state = %ld  children= %d\n",task->comm,task->pid,task->state,num_child);

 }
 
  return 0;
}

//Exit function
void module_end(void)
{
  printk(KERN_INFO "Removing module....\n");
}

MODULE_LICENSE("GPL");

module_init(module_start);
module_exit(module_end);
