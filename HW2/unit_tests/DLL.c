/**********************************************************
* Author: Vishal Vishnani
* Date: 9/6/2017
* File: DLL.c
* Description: source file for Double Linked List
***********************************************************/


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "DLL.h"

/*This function returns the size of double linked list*/
uint32_t find_size(struct Node* n){
  uint32_t count=0;
  while(n!=NULL){
    count++;
    n=n->next;
  }
  return count;
}


/*This function adds a node to linked list at given position*/
dll_state add_node(struct Node** n,uint32_t position,uint32_t value){
  struct Node* n1 = *n;
  struct Node* temp= *n;
  struct Node* new_node=(struct Node*)malloc(sizeof(struct Node));
  new_node->data=value;
  uint32_t count=0;
  struct Node* n_prev=NULL;

  /*Finding the length of linkedlist*/
  uint32_t length=find_size(n1);

  /*Adding a node at starting position of empty linked list*/  
  if(position==0 && n1==NULL){
    new_node->prev=NULL;
    new_node->next=NULL;
    *n=new_node;
    return SUCCESS;
  }
  
  /*Adding a node at starting position of linkedlist which is not empty*/
  else if (position==0 && n1!=NULL){
    new_node->prev=NULL;
    new_node->next=n1;
    n1->prev=new_node;
    *n=new_node;
    return SUCCESS;
  }

  /*Adding a node in between the start position and end position*/
  else if(position>0 && (position <= (length - 1)) && n1!=NULL){
    while(count!=position){
      n_prev=n1;
      n1=n1->next;
      count++;
    }
    new_node->next=n1;
    new_node->prev=n1->prev;
    n1->prev=new_node;
    n_prev->next=new_node;
    *n=temp;
    return SUCCESS;
  }

  /*Adding a node at end of linkedlist*/
  else if(position==(length) && n1!=NULL){
    while((n1->next)!=NULL){
      n1=n1->next;
    }
    new_node->prev=n1;
    new_node->next=NULL;
    n1->next=new_node;  
    *n=temp;
    return SUCCESS;
  }

  /*For any other cases return FAILURE*/
  else{
    return FAILURE;
  }  

}

/*This function removes a node at given position from linked list*/
dll_state remove_node(struct Node** n,uint32_t* data,uint32_t position){
  struct Node* n1 = *n;
  struct Node* temp = n1;
  struct Node* n_prev=NULL;
  struct Node* temp1=NULL;
  uint32_t count=0;
  uint32_t length=find_size(n1);

  /*If linkedlist is empty return FAILURE*/
  if(n1==NULL)
    return FAILURE;

  
  /*Removing an element at first position of linkedlist*/
  if(position==0){
    *data=n1->data;
    n1=n1->next;
    n1->prev=NULL;
    free(temp);
    *n=n1;
    return SUCCESS;
  }

  /*Removing an element between the start position and end position*/
  else if (position>0 && position <= (length - 2)){
    while(count!=position){
      n_prev=n1;
      n1=n1->next;
      count++;
    }
    temp1=n1;
    *data=n1->data;
    n1=n1->next;
    n_prev->next=n1;
    n1->prev=n_prev;
    free(temp1);
    *n=temp;  
    return SUCCESS;
  }

  /*Removing the node at end position*/
  else if(position==(length-1)){
    while((n1->next)!=NULL){
      n_prev=n1;
      n1=n1->next;
    }
    *data=n1->data;
    n_prev->next=NULL;
    free(n1);
    *n=temp;
    return SUCCESS;
  }

  /*For al other cases return failure*/
  else
    return FAILURE;
}


/*This function searches the linked list for a given value and returns the address of that node*/
struct Node* search_node(struct Node* n,uint32_t* position, uint32_t value){
  struct Node* position_node=NULL; 
  uint32_t search=*position;
  while((n->data)!=value){
    n=n->next;
    search++; 
  }
  position_node=n;
  *position=search;
  return position_node;
 
}


/*This function displays the contents of linkedlist*/
void printlist(struct Node* n){
  while(n!=NULL){
    printf("%d ",n->data);
    n=n->next;
  }
}

/*This function frees the memory taken by linked list*/
dll_state destroy(struct Node* n){
  if (n==NULL)
    return FAILURE;

  while(n!=NULL){
    free(n);
    n=n->next;
  }
  return SUCCESS;
}

/*
int main(){
  struct Node* head=NULL;
  printf("\nLinkedlist after adding value 0 at start:\n");
  
  //Adding an element at first position of empty linked list, if successfull thereturn status is 0
  uint32_t state=add_node(&head,0,0);
  printlist(head);
  printf("\nReturn status after adding is %d \n",state);

  
//Adding an element at end position of linkedlist,if successful the return status is 0
  printf("\nLinkedlist after adding value 2 at position 1\n");
  add_node(&head,1,2);
  printlist(head);
  printf("\nReturn status after adding is %d \n",state);
  

  //Adding an element between the start position and end position
  printf("\nLinkedlist after adding value 3 in between\n");
  add_node(&head,1,3);
  printlist(head); 
  printf("\nReturn status after adding is %d \n",state);

  uint32_t position=0;
  struct Node* position_node=search_node(head,&position,2);
  uint32_t data_verify=position_node->data;
  printf("\nSearch value 2 in the linked list");
  printf("\nValue %d is at position: %d",data_verify,position);
  printf("\nValue %d is at address %d",data_verify,(uint32_t*)position_node); 

  //Removing an element at start position, if successfull value returned is 0
  printf("\nLinkedlist after removing node at position 0\n");
  uint32_t data_removed=0;
  state=remove_node(&head,&data_removed,0);
  printlist(head);
  printf("\nReturn status after removing is %d \n",state);
  printf("Removed Value: %d\n",data_removed);

  //Removing an element between start position and end position
  printf("\nLinkedlist after removing node at position 1\n");
  state=remove_node(&head,&data_removed,1);
  printlist(head);
  printf("\nRemoved Value: %d\n",data_removed);
  printf("Return status after removing is %d \n",state);



  printf("\nLinkedlist After deallocating memory\n");
  state=destroy(head);
  printf("Return status after Deallocating is %d \n",state);
  
}
*/
