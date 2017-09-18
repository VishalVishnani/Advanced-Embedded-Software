/**********************************************************
* Author: Vishal Vishnani
* Date: 9/6/2017
* File: CB.c
* Description: Source file for Circular Buffer
***********************************************************/

/*Includes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "CB.h"


/*Buffer size*/
#define BUFFER_SIZE 6
  

/*Function which assigns dynamic memory to circular buffer and circular buffer structure. Also, initializes circular buffer*/
cbuff_state allocate_buffer(uint32_t** cbuffer, CircBuff** CircBuffTR, uint32_t length_buff){
  
  /*dynamic memory allocation for buffer*/
  *cbuffer=(uint32_t*)calloc(length_buff,sizeof(uint32_t));
  
  CircBuff* CircBuffTR1=*CircBuffTR;

  /*dynamic memory allocation for structure*/
  CircBuffTR1=(CircBuff*)calloc(1,sizeof(CircBuff));

  /*Initialize buffer*/
  CircBuffTR1->buffer=*cbuffer;
  CircBuffTR1->head=CircBuffTR1->buffer;
  CircBuffTR1->tail=CircBuffTR1->head;
  CircBuffTR1->count=0;
  CircBuffTR1->length=length_buff;  
  *CircBuffTR=CircBuffTR1;

  if(*cbuffer==NULL || CircBuffTR1==NULL)
    return BUFFER_ALLOCATION_FAILURE;
  else
    return NO_ERROR;
}


/*Function which adds items to circular buffer*/
cbuff_state buffer_add(CircBuff* CircBuffTR,uint32_t* data, uint32_t values_to_add){
  
  if(CircBuffTR==NULL){
    return NULL_POINTER;
  }

  if(values_to_add==0)
    return NO_LENGTH;
  
  while(values_to_add){
    if((CircBuffTR->count)!=(CircBuffTR->length)){
      *(CircBuffTR->head)=*data;
      (CircBuffTR->head)++;
      (CircBuffTR->count)++;
//      printf("\nAdded Value %d to buffer",*data); 

      /*wrap around condition*/
      if((CircBuffTR->head)>((CircBuffTR->buffer)+(CircBuffTR->length)-1)){
        CircBuffTR->head=CircBuffTR->buffer;
      }
      (CircBuffTR->cbuff_state)=AVAILABLE;
    }
    else{
      CircBuffTR->cbuff_state=BUFFER_FULL;
      return CircBuffTR->cbuff_state; 
    }
    data++;
    values_to_add--;  
  }
  return CircBuffTR->cbuff_state;
}


/*Function which removes item from circular buffer*/
cbuff_state buffer_remove(CircBuff* CircBuffTR,uint32_t values_to_remove){  
  if(CircBuffTR==NULL)
    return NULL_POINTER;

  if(values_to_remove==0)
    return NO_LENGTH;

  while(values_to_remove){
    if((CircBuffTR->count)!=0){
  //    printf("\nRemoved Value %d from buffer",*(CircBuffTR->tail));
      *(CircBuffTR->tail)=0;
      CircBuffTR->tail++;
      CircBuffTR->count--;


      /*wrap around condition*/
      if((CircBuffTR->tail)>((CircBuffTR->buffer)+(CircBuffTR->length)-1 )) {
        CircBuffTR->tail=CircBuffTR->buffer;
      }
      CircBuffTR->cbuff_state=NO_ERROR; 

    }
    else{
      CircBuffTR->cbuff_state=BUFFER_EMPTY;
      return CircBuffTR->cbuff_state;
    }
    values_to_remove--; 
  }
  return CircBuffTR->cbuff_state;
}


/*Function which checks if the Circular buffer is full*/
cbuff_state buffer_full(CircBuff* CircBuffTR){
  if(CircBuffTR==NULL)
    return NULL_POINTER;
  
  if((CircBuffTR->count)==(CircBuffTR->length))
    CircBuffTR->cbuff_state=BUFFER_FULL;
  else
    CircBuffTR->cbuff_state=AVAILABLE;

  return CircBuffTR->cbuff_state;
}


/*Function which checks  if the circular buffer is empty*/
cbuff_state buffer_empty(CircBuff* CircBuffTR){
  if(CircBuffTR==NULL)
    return NULL_POINTER;

  if(CircBuffTR->count==0)
    CircBuffTR->cbuff_state=BUFFER_EMPTY;
  else
    CircBuffTR->cbuff_state=AVAILABLE;

  return CircBuffTR->cbuff_state;
}


/*Function which frees circular buffer and structure*/
cbuff_state cb_destroy(CircBuff* CircBuffTR,uint32_t* c_buffer){
  free((void*)c_buffer);
  free((void*)CircBuffTR);

  return NO_ERROR;
}


/*Function which returns the size of circular buffer*/
uint32_t current_size(CircBuff* CircBuffTR){
  return CircBuffTR->count;
}


/*Function which displays the contents of circular buffer from tail until count value*/
void dump_buffer(CircBuff* CircBuffTR){
  volatile uint32_t * temp=CircBuffTR->tail;
  uint32_t length=CircBuffTR->count;
  printf("\nBuffer Contents: ");
  while(length){
    if((temp)>((CircBuffTR->buffer)+(CircBuffTR->length)-1 )) {
        temp=CircBuffTR->buffer;
    }

    printf("%d ",*temp);
    temp++;
    length--;
  }
}

/*int main(){
  uint32_t length_buff=BUFFER_SIZE;

  //Buffer pointer
  uint32_t *cbuffer=NULL;

  //Buffer structure pointer
  CircBuff* CircBuff1=NULL;

  //Buffer and structure memory allocation, if no error the state returned is 0
  uint32_t state = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  printf("\nBuffer state after allocation %d\n",state);
 
  uint32_t add_data[6]={12,13,14,15,16,17};

  //Adding elements to buffer, if all elements are added successfully, the value returned is 7
  state=buffer_add(CircBuff1,(uint32_t *)&add_data,6);
  printf("\nBuffer_state after adding = %d\n",state);

  //Removing elements from buffer, if all elements are removed successfully, the value returned is 0
  state=buffer_remove(CircBuff1,4);
  printf("\nBuffer_state after removing = %d\n",state);

 
  //If buffer is empty the state returned is 3
  state=buffer_remove(CircBuff1,3);
  printf("\nBuffer_state after removing = %d\n",state);


  //Print buffer contents
  dump_buffer(CircBuff1);
  
  //display current count value of buffer
  state= current_size(CircBuff1);
  printf("\nCurrent size = %d\n", state);
  
  //Adding elements to buffer, if buffer is full the value returned is 2
  uint32_t add_data1[6]={22,23,24,26,27,28};
  state=buffer_add(CircBuff1,(uint32_t*)&add_data1,6);
  printf("\nBuffer_state after adding = %d\n",state);

  //Free circular buffer memory, if successfull value returned is 0
  state=cb_destroy(CircBuff1,cbuffer);
  printf("\nBuffer_state after deallocation = %d\n",state);
  
}
*/
