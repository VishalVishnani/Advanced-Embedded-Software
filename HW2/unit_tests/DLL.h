/**********************************************************
* Author: Vishal Vishnani
* Date: 9/6/2017
* File: DLL.h
* Description: Header file for Double Linked List
***********************************************************/

#include <stdint.h>

#ifndef SOURCES_DLL_H_
#define SOURCES_DLL_H_

/*Linked list node structure*/
struct Node{
  uint32_t data;
  struct Node* next;
  struct Node* prev;
};

/*Enums used to return states*/
typedef enum DLL_states{
  SUCCESS,
  FAILURE
}dll_state; 



/**********************************************************
* find_size() - Returns the size of double linked list
*
* uint32_t - size of linked list returned
*
* struct Node* n - Pointer to head of double linked list
**********************************************************/
uint32_t find_size(struct Node* n);




/**********************************************************
* add_node() - Adds a node to double linked list
*
* uint32_t position - position at which the node is to be 
*                     inserted
* uint32_t value - Value to be assigned new node
*
* dll state - Enum returned depending on whether node
*             was successfully added
*
* struct Node* n - Pointer to head of double linked list
**********************************************************/
dll_state add_node(struct Node** n,uint32_t position,uint32_t value);



/**********************************************************
* remove_node() - remove a node from double linked list
*
* uint32_t position - position from which the node is to be 
*                     removed
* uint32_t* data - Value at that node is stored in data
*
* dll state - Enum returned depending on whether node
*             was successfully removed
*
* struct Node* n - Pointer to head of double linked list
**********************************************************/
dll_state remove_node(struct Node** n,uint32_t* data,uint32_t position);



/**********************************************************
* search_node() - Search a node of given value  in double 
*                 linked list
*
* uint32_t position - position at which the node is found
*
* uint32_t value - Value to be searched
*
* struct Node* - Address whether the given node was found
*                is returned
*
* struct Node* n - Pointer to head of double linked list
**********************************************************/
struct Node*  search_node(struct Node* n,uint32_t* position, uint32_t value);


/**********************************************************
* printlist() - prints the contents of double linked list
*
* struct Node* n - Pointer to head of double linked list
**********************************************************/
void printlist(struct Node* n);



/**********************************************************
* destroy_node() - Destroys and frees the double linked list
*
* dll state - Enum returned depending on whether freeing
              memory was successful
*
* struct Node* n - Pointer to head of double linked list
**********************************************************/
dll_state destroy(struct Node* n);


#endif

