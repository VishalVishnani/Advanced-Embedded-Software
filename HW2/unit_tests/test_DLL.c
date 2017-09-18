#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "DLL.h"

void test_DLL_add_emptylist(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  assert_int_equal(status,SUCCESS);
}

void test_DLL_add_empty_position_not0(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,1,0);
  assert_int_equal(status,FAILURE);
}

void test_DLL_add_position0(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  assert_int_equal(status,SUCCESS);

}

void test_DLL_add_end(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  assert_int_equal(status,SUCCESS);
}

void test_DLL_add_bet(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,1,8);
  assert_int_equal(status,SUCCESS);
}

void test_DLL_add_after_endpos(void ** state){
  struct Node* head=NULL;
  dll_state status=add_node(&head,0,0);
  status=add_node(&head,0,2);
  status=add_node(&head,2,5);
  status=add_node(&head,4,8);
  assert_int_equal(status,FAILURE);
}


void test_DLL_remove_NULL(void ** state){
   struct Node* head=NULL;
   uint32_t data_removed=0;
   dll_state status=remove_node(&head,&data_removed,0);
   assert_int_equal(status,FAILURE);
}

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

void test_DLL_destroy_null(void ** state){
  struct Node* head=NULL;
  dll_state status=destroy(head);
  assert_int_equal(status,FAILURE);
}

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

void test_find_size(void ** state){
  struct Node* head=NULL;
  add_node(&head,0,0);
  add_node(&head,0,2);
  add_node(&head,2,5);
  add_node(&head,1,8);
  uint32_t size=find_size(head);
  assert_int_equal(size,4);
}


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

