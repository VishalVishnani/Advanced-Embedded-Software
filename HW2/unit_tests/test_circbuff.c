#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "CB.h"

uint32_t length_buff=10;
uint32_t data[11]={11,12,13,14,15,22,23,24,25,26};


void test_circbuf_allocate(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  assert_int_equal(status,NO_ERROR);

}

void test_circbuf_invalid_pointer(void ** state){
  cbuff_state status=buffer_full(NULL);
  assert_int_equal(status,NULL_POINTER);
}

void test_circbuf_add(void ** state){
  uint32_t *cbuffer=NULL; 
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status=buffer_add(CircBuff1,(uint32_t *)data,6);
  assert_int_equal(status,AVAILABLE);

}

void test_circbuf_not_full(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status=buffer_add(CircBuff1,(uint32_t *)data,9);
  status=buffer_full(CircBuff1);
  assert_int_equal(status,AVAILABLE);

}

void test_circbuf_full(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status=buffer_add(CircBuff1,(uint32_t *)data,10);
  status=buffer_full(CircBuff1);
  assert_int_equal(status,BUFFER_FULL);

}


void test_circbuf_empty(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status=buffer_empty(CircBuff1);
  assert_int_equal(status,BUFFER_EMPTY);

}

void test_circbuf_not_empty(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status=buffer_add(CircBuff1,(uint32_t *)data,1);
  status=buffer_empty(CircBuff1);
  assert_int_equal(status,AVAILABLE);

}



void test_circbuf_remove(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status=buffer_add(CircBuff1,(uint32_t *)data,10);
  status=buffer_remove(CircBuff1,3);
  assert_int_equal(status,NO_ERROR);

}

void test_circbuf_remove_nullptr(void ** state){
//  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status=buffer_remove(CircBuff1,3);
  assert_int_equal(status,NULL_POINTER);

}

void test_circbuf_remove_nolength(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status=buffer_add(CircBuff1,(uint32_t *)data,10);
  status=buffer_remove(CircBuff1,0);
  assert_int_equal(status,NO_LENGTH);

}


void test_circbuf_add_nullptr(void ** state){
  CircBuff* CircBuff1=NULL;
  cbuff_state status=buffer_add(CircBuff1,(uint32_t*)data,3);
  assert_int_equal(status,NULL_POINTER);
}

void test_circbuf_add_nolength(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status=buffer_add(CircBuff1,(uint32_t *)data,0);
  assert_int_equal(status,NO_LENGTH);


}

void test_circbuf_free(void ** state){
  uint32_t *cbuffer=NULL;
  CircBuff* CircBuff1=NULL;
  cbuff_state status = allocate_buffer(&cbuffer,&CircBuff1,length_buff);
  status= cb_destroy(CircBuff1,cbuffer);
  assert_int_equal(status,NO_ERROR);

}

int main(int argc, char** argv)
{
  const struct CMUnitTest tests[]={
    cmocka_unit_test(test_circbuf_invalid_pointer),
    cmocka_unit_test(test_circbuf_allocate),
    cmocka_unit_test(test_circbuf_add),
    cmocka_unit_test(test_circbuf_full),
    cmocka_unit_test(test_circbuf_not_full),
    cmocka_unit_test(test_circbuf_not_empty),
    cmocka_unit_test(test_circbuf_empty),
    cmocka_unit_test(test_circbuf_remove),
    cmocka_unit_test(test_circbuf_remove_nullptr),
    cmocka_unit_test(test_circbuf_remove_nolength),
    cmocka_unit_test(test_circbuf_add_nullptr),
    cmocka_unit_test(test_circbuf_add_nolength),
    cmocka_unit_test(test_circbuf_free),
  };

  return cmocka_run_group_tests(tests,NULL,NULL);


}

