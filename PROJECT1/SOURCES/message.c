
#include <stdint.h>
#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>

#define MSG_SIZE 4096
#define NO_OF_MSG 10
#define QUEUE "/queue"
#define FLAG (O_RDWR | O_CREAT)

int main(){
  struct mq_attr attr;
  mqd_t mqdes;
  attr.mq_maxmsg=NO_OF_MSG;
  attr.mq_msgsize=MSG_SIZE;
  attr.mq_flags=0;

  mq_unlink(QUEUE);

  mqdes=mq_open(QUEUE, FLAG, 0644, &attr);

  if(mqdes==-1){
    printf("\nError mq_open\n");
    exit(1);
  }

  mq_getattr(mqdes,&attr);
  
  printf("\nMessages currently on the queue %ld\n",attr.mq_curmsgs);
  printf("\nSize of the queue %ld\n",attr.mq_maxmsg);

  int8_t message[]="This is first msg I am trying";
  
  if(mq_send(mqdes,(const int8_t*)message,sizeof(message)-1,0)==-1){
    printf("\nError in mq_send\n");
    exit(1);
  }

  mq_getattr(mqdes,&attr);

  printf("\nAfter send Messages currently on the queue %ld\n",attr.mq_curmsgs);
  printf("\nSize of the queue %ld\n",attr.mq_maxmsg);

  int8_t receive[4096];
  int8_t n=mq_receive(mqdes,(int8_t*)receive,MSG_SIZE,NULL);

  printf("\nReceive = %s\n",receive);

  mq_getattr(mqdes,&attr);

  printf("\nAfter receive Messages currently on the queue %ld\n",attr.mq_curmsgs);
  printf("\nSize of the queue %ld\n",attr.mq_maxmsg);

  
  mq_close(mqdes);

  return 0;

}
