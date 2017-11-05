#ifndef QUEUE_H
#define QUEUE_H

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

#define MSG_SIZE 4096
#define NO_OF_MSG 10
#define QUEUE_TEMP_W "/tempqueue1"
#define QUEUE_LIGHT_W "/lightqueue1"
#define QUEUE_TEMP_R "/tempqueue2"
#define QUEUE_LIGHT_R "/lightqueue2"
#define QUEUE_MAIN "/mainqueue"
#define QUEUE_DECISION "/decisionqueue"
#define FLAG (O_RDWR | O_CREAT)


struct mq_attr attr_temp_w;
mqd_t mqdes_temp_w;


struct mq_attr attr_light_w;
mqd_t mqdes_light_w;


struct mq_attr attr_temp_r;
mqd_t mqdes_temp_r;


struct mq_attr attr_light_r;
mqd_t mqdes_light_r;


struct mq_attr attr_main;
mqd_t mqdes_main;

struct mq_attr attr_decision;
mqd_t mqdes_decision;

void initialize_queues(void);


#endif
