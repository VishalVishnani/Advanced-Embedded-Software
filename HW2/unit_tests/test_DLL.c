/**********************************************************
* Author: Vishal Vishnani
* Date: 9/6/2017
* File: test_DLL.c
* Description: Test file for Double Linked List
***********************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "DLL.h"

/*This function tests if adding to an empty list at position 0 is successful*/
void test_DLL_add_emptylist(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  assert_int_equal(status,SUCCESS);
}

/*This function tests if adding to an empty list at position other than 0 fails*/
void test_DLL_add_empty_position_not0(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,1,0);
  assert_int_equal(status,FAILURE);
}


/*This function tests if adding to a non-empty list at 0 is successful*/
void test_DLL_add_position0(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  assert_int_equal(status,SUCCESS);

}

/*This function tests if adding an element at end position of linkedlist is successful*/
void test_DLL_add_end(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  assert_int_equal(status,SUCCESS);
}


/*This function tests if adding an element between start and end position of linked position is successful*/
void test_DLL_add_bet(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,1,8);
  assert_int_equal(status,SUCCESS);
}


/*This function tests if adding an element after the end position of linkedlist fails*/
void test_DLL_add_after_endpos(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,4,8);
  assert_int_equal(status,FAILURE);
}


/*This function tests if removing an element from empty linked list fails*/
void test_DLL_remove_NULL(void ** state){
   struct Node* head=NULL;
   uint32_t data_removed=0;
   dll_state status=remove_node(&head,&data_removed,0);
   assert_int_equal(status,FAILURE);
}

/*This function tests if removing node at position0 of non-empty linkedlist is successful*/
void test_DLL_remove_position0(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,1,8);
  uint32_t data_removed=0;
  status=remove_node(&head,&data_removed,0);
  assert_int_equal(status,SUCCESS);
}

/*This function tests if removing node at end position of linkedlist is successful*/
void test_DLL_remove_end(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,1,8);
  uint32_t data_removed=0;
  status=remove_node(&head,&data_removed,3);
  assert_int_equal(status,SUCCESS);
}

/*This function tests if removing node between start and end position of linkedlist is successful*/
void test_DLL_remove_bet(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,1,8);
  uint32_t data_removed=0;
  status=remove_node(&head,&data_removed,1);
  assert_int_equal(status,SUCCESS);
}

/*This function tests if removing a node after the end position of linked list fails*/
void test_DLL_remove_after_endpos(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,1,8);
  uint32_t data_removed=0;
  status=remove_node(&head,&data_removed,5);
  assert_int_equal(status,FAILURE);

}

/*This function tests if the destroy function returns FAILURE, if an invalid pointer to linked list is passed*/
void test_DLL_destroy_null(void ** state){
  struct Node* head=NULL;
  dll_state status=destroy(head);
  assert_int_equal(status,FAILURE);
}

/*This function tests the destroy function*/ 
void test_DLL_destroy(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,1,8);
  uint32_t data_removed=0;
  status=remove_node(&head,&data_removed,1);
  status=destroy(head);
  assert_int_equal(status,SUCCESS);
}

/*This function tests if the size returned by find_size function is correct*/
void test_find_size(void ** state){
  struct Node* head=NULL;
  add_node(&head,0,0);
  add_node(&head,0,2);
  add_node(&head,2,5);
  add_node(&head,1,8);
  uint32_t size=find_size(head);
  assert_int_equal(size,4);
}

/*This function tests the search_node function*/
void test_search_node(void ** state){
  struct Node* head=NULL;
  add_node(&head,0,0);
  uint32_t position=0;
  struct Node* position_node=search_node(head,&position,0);
  assert_int_equal(head,position_node);  

}

int main(int argc, char** argv)
{
  const struct CMUnitTest tests[]={
    cmocka_unit_test(test_DLL_add_emptylist),
    cmocka_unit_test(test_DLL_add_empty_position_not0),
    cmocka_unit_test(test_DLL_add_position0),
    cmocka_unit_test(test_DLL_add_end),
    cmocka_unit_test(test_DLL_add_bet),
    cmocka_unit_test(test_DLL_remove_NULL),
    cmocka_unit_test(test_DLL_remove_position0),
    cmocka_unit_test(test_DLL_remove_end),
    cmocka_unit_test(test_DLL_remove_bet),
    cmocka_unit_test(test_DLL_remove_after_endpos),
    cmocka_unit_test(test_DLL_add_after_endpos),
    cmocka_unit_test(test_DLL_destroy_null),
    cmocka_unit_test(test_DLL_destroy),
    cmocka_unit_test(test_find_size),
    cmocka_unit_test(test_search_node),
  };

  return cmocka_run_group_tests(tests,NULL,NULL);


}

