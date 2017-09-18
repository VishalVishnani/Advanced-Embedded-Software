/**********************************************************
* Author: Vishal Vishnani
* Date: 9/6/2017
* File: CB.h
* Description: Header file for Circular Buffer
***********************************************************/    

#include <stdint.h>

#ifndef SOURCES_CB_H_
#define SOURCES_CB_H_

/*Enums for return states*/
typedef enum buffer_states{
  NO_ERROR=0,
  ERROR,
  BUFFER_FULL,
  BUFFER_EMPTY,
  NULL_POINTER,
  NO_LENGTH,
  BUFFER_ALLOCATION_FAILURE,
  AVAILABLE
}cbuff_state;

/*Circular Buffer structure*/
typedef struct CircBuf_t{
  volatile uint32_t* buffer;
  volatile uint32_t* head;
  volatile uint32_t* tail;
  uint32_t length;
  volatile uint32_t count;
  cbuff_state cbuff_state;

}CircBuff;



/**********************************************************
* allocate_buffer() - Assigns dynamic memory to circular
*                     buffer and circular buffer structure,
*                     also initializes circular buffer
*
* cbuff_state - enum to check if buffer and structure were
*               allocated memory
*
* uint32_t** cbuffer - Double pointer which points to circular
*                     buffer address
* 
* CircBuff** CircBuffTR - Double pointer which points to
*                         structure address
*
* uint32_t length_buff - Length of Circular Buffer
**********************************************************/
cbuff_state allocate_buffer(uint32_t** cbuffer, CircBuff** CircBuffTR, uint32_t length_buff);



/**********************************************************
* buffer_add() - Adds an item to circular buffer
*
* cbuff_state - enum to check if the item was added
*
* uint32_t* data - Points to the data which is to be added
* 
* CircBuff* CircBuffTR - Pointer which points to structure
*
* uint32_t values_to_add - Number of values to add
**********************************************************/
cbuff_state buffer_add(CircBuff* CircBuffTR,uint32_t* data, uint32_t values_to_add);



/**********************************************************
* buffer_remove() - Removes an item from circular buffer
*
* cbuff_state - enum to check if the item was removed
* 
* CircBuff* CircBuffTR - Pointer which points to structure
*
* uint32_t values_to_remove - Number of values to remove
**********************************************************/
cbuff_state buffer_remove(CircBuff* CircBuffTR,uint32_t values_to_remove);



/**********************************************************
* buffer_full() - Checks if the buffer is full
*
* cbuff_state - enum returned depending on whether buffer
*               is full or not
* 
* CircBuff* CircBuffTR - Pointer which points to structure
**********************************************************/
cbuff_state buffer_full(CircBuff* CircBuffTR);



/**********************************************************
* buffer_empty() - Checks if the buffer is empty
*
* cbuff_state - enum returned depending on whether buffer
*               is empty or not
* 
* CircBuff* CircBuffTR - Pointer which points to structure
**********************************************************/
cbuff_state buffer_empty(CircBuff* CircBuffTR);




/**********************************************************
* cb_destroy() - Frees the circular buffer and structure
*
* cbuff_state - enum to check if the memory was freed
* 
* CircBuff* CircBuffTR - Pointer which points to structure
*
* uint32_t* c_buffer - Pointer to Circular buffer
**********************************************************/
cbuff_state cb_destroy(CircBuff* CircBuffTR,uint32_t* c_buffer);



/**********************************************************
* current_size() - Returns the size of circular buffer
*
* uint32_t - circular buffer size returned
* 
* CircBuff* CircBuffTR - Pointer which points to structure
**********************************************************/
uint32_t current_size(CircBuff* CircBuffTR);



/**********************************************************
* dump_buffer() - Displays circular buffer contents
*
* CircBuff* CircBuffTR - Pointer which points to structure
**********************************************************/
void dump_buffer(CircBuff* CircBuffTR);


#endif

