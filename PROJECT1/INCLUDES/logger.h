/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : logger.h
* Description : Header file for Logger
******************************************************************************************/



#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>


/*Enum for logger_level*/
typedef enum check_level{
  INFO=0,
  ERROR,
  SENSOR_VALUE,
  ALERT
}level;


/*Enum for Task ID */
typedef enum task_type{
  MAIN_TASK=0,
  TEMP_TASK,
  LIGHT_TASK,
  LOGGER_TASK,
  DECISION_TASK
}task_id;


/*Structure for LOG packet*/
typedef struct log_packet{
  level log_level;
  task_id log_id;
  uint8_t *timestamp;
  uint8_t log_message[300];
  char data[300];
}log;


/*Structure for sending async request*/
typedef struct log_req{
  task_id src_id;
  task_id dst_id;
  uint8_t* timestamp;
  uint8_t command;
  uint8_t delay;
}request_log;


#endif

