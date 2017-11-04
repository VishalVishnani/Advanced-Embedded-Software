
#include "queue.h"
#include <stdlib.h>

void initialize_queues(void){
  attr_temp_w.mq_maxmsg=NO_OF_MSG;
  attr_temp_w.mq_msgsize=sizeof(log);
  attr_temp_w.mq_flags=0;

  attr_light_w.mq_maxmsg=NO_OF_MSG;
  attr_light_w.mq_msgsize=sizeof(log);
  attr_light_w.mq_flags=0;

  attr_temp_r.mq_maxmsg=NO_OF_MSG;
  attr_temp_r.mq_msgsize=sizeof(log);
  attr_temp_r.mq_flags=0;

  attr_light_r.mq_maxmsg=NO_OF_MSG;
  attr_light_r.mq_msgsize=sizeof(log);
  attr_light_r.mq_flags=0;

  attr_main.mq_maxmsg=NO_OF_MSG;
  attr_main.mq_msgsize=sizeof(log);
  attr_main.mq_flags=0;

  mq_unlink(QUEUE_TEMP_W);
  mq_unlink(QUEUE_LIGHT_W);

  mq_unlink(QUEUE_TEMP_R);
  mq_unlink(QUEUE_LIGHT_R);

  mq_unlink(QUEUE_MAIN);

  mqdes_temp_w=mq_open(QUEUE_TEMP_W, FLAG, 0644, &attr_temp_w);

  if(mqdes_temp_w==-1){
    printf("\nError mq_open temp_w\n");
    exit(1);
  }

  mqdes_light_w=mq_open(QUEUE_LIGHT_W, FLAG, 0644, &attr_light_w);

  if(mqdes_light_w==-1){
    printf("\nError mq_open light_w\n");
    exit(1);
  }


  mqdes_temp_r=mq_open(QUEUE_TEMP_R, FLAG, 0644, &attr_temp_r);

  if(mqdes_temp_r==-1){
    printf("\nError mq_open temp_r\n");
    exit(1);
  }

  mqdes_light_r=mq_open(QUEUE_LIGHT_R, FLAG, 0644, &attr_light_r);

  if(mqdes_light_r==-1){
    printf("\nError mq_open light_r\n");
    exit(1);
  }

  mqdes_main=mq_open(QUEUE_MAIN, FLAG, 0644, &attr_main);
  if(mqdes_main==-1){
    printf("\nError mq_open main\n");
    exit(1);
  }

  mq_getattr(mqdes_temp_w,&attr_temp_w);
  mq_getattr(mqdes_light_w,&attr_light_w);

  mq_getattr(mqdes_temp_r,&attr_temp_r);
  mq_getattr(mqdes_light_r,&attr_light_r);

  mq_getattr(mqdes_main,&attr_main);

}
