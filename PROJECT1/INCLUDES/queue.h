/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : queue.h
* Description : Header file for Message Queues
                -initilialize_queues()
******************************************************************************************/




#ifndef QUEUE_H
#define QUEUE_H

/*INCLUDES*/
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

/*MACROS*/
#define MSG_SIZE 4096
#define NO_OF_MSG 10
#define QUEUE_TEMP_W "/tempqueue1"
#define QUEUE_LIGHT_W "/lightqueue1"
#define QUEUE_TEMP_R "/tempqueue2"
#define QUEUE_LIGHT_R "/lightqueue2"
#define QUEUE_MAIN "/mainqueue"
#define QUEUE_DECISION "/decisionqueue"
#define FLAG (O_RDWR | O_CREAT)


/*Queue for Temperature*/
struct mq_attr attr_temp_w;
mqd_t mqdes_temp_w;


/*Queue for Light*/
struct mq_attr attr_light_w;
mqd_t mqdes_light_w;

/*Queue for async request for temperature*/
struct mq_attr attr_temp_r;
mqd_t mqdes_temp_r;

/*Queue for async request for light*/
struct mq_attr attr_light_r;
mqd_t mqdes_light_r;

/*Queue for main*/
struct mq_attr attr_main;
mqd_t mqdes_main;

/*Queue for decision task*/
struct mq_attr attr_decision;
mqd_t mqdes_decision;

/*Function to initialize queues*/
void initialize_queues(void);


#endif
