#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>



typedef enum check_level{
  INFO=0,
  ERROR,
  SENSOR_VALUE,
  ALERT
}level;



typedef enum task_type{
  MAIN_TASK=0,
  TEMP_TASK,
  LIGHT_TASK,
  LOGGER_TASK,
  DECISION_TASK
}task_id;


typedef struct log_packet{
  level log_level;
  task_id log_id;
  uint8_t *timestamp;
  uint8_t log_message[100];
  char data[100];
}log;



typedef struct log_req{
  task_id src_id;
  task_id dst_id;
  uint8_t* timestamp;
  uint8_t command;
  uint8_t delay;
}request_log;

/*
char logger_level[4][15]={"INFO","ERROR","SENSOR_VALUE","ALERT"};
char task_no[5][15]={"MAIN_TASK","TEMP_TASK","LIGHT_TASK","LOGGER_TASK","DECISION_TASK"};
*/

#endif

