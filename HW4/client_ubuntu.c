/*INCLUDES*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*Enum to check if led is on/off*/
typedef enum ledstate{
  LED_ON = 1,
  LED_OFF = 0
}state;



/*Structure to hold the device parameters*/
typedef struct packet_t
{
  int state_led;
  int period;
  int dutycycle;
}packet;



int main(int argc, char* argv[])
{
  int sockfd, port_number, n;

  struct sockaddr_in server_address;
  struct hostent *server;


  if(argc<3){
    printf("\nUSAGE %s hostname port\n",argv[0]);
    exit(1);
  }

  port_number = atoi(argv[2]);

  /*Create socket*/
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0){
    printf("\nERROR: SOCKET CREATION\n");
    exit(1);
  }

  /*Check host*/
  server = gethostbyname(argv[1]);

  if(server == NULL)
  {
    printf("\nERROR: NO HOST\n");
    exit(1);
  }

  bzero((char *)&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);

  server_address.sin_port = htons(port_number);

  /*Connect*/
  if(connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
  {
    printf("\nERROR: CONNECTION\n");
  }

  char option[20] = {0};
  char command_query[10] = {0};
  packet packet_send;

  /*Deafault period and dutycyle*/
  uint32_t default_period = 1;
  uint32_t default_dutycycle = 50;
  while(1){

    printf("\nENTER COMMAND OR QUERY\n");

    /*Wait for command from user*/
    scanf( "%[^\n]s",command_query);
    getchar();

    if(strncmp(command_query,"COMMAND",strlen("COMMAND")) == 0){

      write(sockfd,"COMMAND",strlen("COMMAND"));
      usleep(1000);

      /*print the options,take the command input and filename*/
      printf("\nCOMMAND OPTIONS:\n");
      printf("1. STATE [ON/OFF]\n");
      printf("2. PERIOD [1-3]\n");
      printf("3. DUTYCYCLE [1-100]\n");

      scanf( "%[^\n]s",option);
      getchar();
 
      if(strncmp(option,"STATE",strlen("STATE")) == 0){

        char *str1 = strtok(option, " ");
        str1 = strtok(NULL, " ");
        if(strncmp(str1,"ON", strlen("ON")) == 0){
          packet_send.state_led = LED_ON;
          packet_send.period = 0;
          packet_send.dutycycle = 0;
        }
        else if(strncmp(str1,"OFF", strlen("OFF")) == 0)
        {
          packet_send.state_led = LED_OFF;
          packet_send.period = 0;
          packet_send.dutycycle = 0;
        }
      }

      else if(strncmp(option,"PERIOD",strlen("PERIOD")) == 0){
        char *str2 = strtok(option, " ");
        str2 = strtok(NULL, " ");
        int periodled = atoi(str2);
        packet_send.state_led = LED_ON;
        packet_send.period = periodled;
        default_period = periodled;
        packet_send.dutycycle = default_dutycycle;

      }
      else if(strncmp(option,"DUTYCYCLE",strlen("DUTYCYCLE")) == 0){
        char *str3 = strtok(option, " ");
        str3 = strtok(NULL, " ");
        int dutycycle_led = atoi(str3);
        if(dutycycle_led < 0 || dutycycle_led > 100){
          printf("\nERROR: Duty cycle value out of bounds \n");
          continue;
        }
        packet_send.state_led = LED_ON;
        packet_send.period = default_period;
        default_dutycycle = dutycycle_led;
        packet_send.dutycycle = dutycycle_led; 
      }

      else{ 
        printf("\nERROR: WRONG COMMAND\n");
        continue;
      }

      printf("\nSTATE: %d\n",packet_send.state_led);
      printf("\nPERIOD: %d\n",packet_send.period);
      printf("\nDUTY CYCLE: %d\n",packet_send.dutycycle);

      //send the packet after receiving the values from the user*/
      write(sockfd,&packet_send,sizeof(packet_send));
    }
    else if(strncmp(command_query,"QUERY" , strlen("QUERY")) == 0 ){
      packet recv_packet;
      write(sockfd,"QUERY",strlen("QUERY"));
      usleep(10000);
      /*print the options,take the command input and filename*/
      printf("\nOPTIONS: \n");
      printf("1. STATE\n");
      printf("2. PERIOD\n");
      printf("3. DUTYCYCLE\n");
      printf("4. ALL\n");

      scanf( "%[^\n]s",option);
      getchar();
      if(strncmp(option,"STATE",strlen("STATE")) == 0){

        /*Read the value from the socket*/
        read(sockfd,&recv_packet,sizeof(recv_packet));
        if(recv_packet.state_led == 1){
          printf("\nSTATE: ON\n");
        }
        else if(recv_packet.state_led == 0){
          printf("\nSTATE: OFF \n");
        }
      }


      else if(strncmp(option,"PERIOD",strlen("PERIOD")) == 0){

        /*Read the value from socket*/
        read(sockfd,&recv_packet,sizeof(recv_packet));
        printf("\nPERIOD: %d\n",recv_packet.period);
      
      }

      else if(strncmp(option,"DUTYCYCLE",strlen("DUTYCYCLE")) == 0){

        /*Read the value from socket*/
        read(sockfd,&recv_packet,sizeof(recv_packet));
        printf("\nDUTYCYCLE %d \n",recv_packet.dutycycle);
      }


      else if(strncmp(option,"ALL",strlen("ALL")) == 0){
        /*Read the value from socket*/
        read(sockfd,&recv_packet,sizeof(recv_packet));
        if(recv_packet.state_led == 1)
        {
          printf("\nSTATE: ON\n");
        }
        else if(recv_packet.state_led == 0)
        {
          printf("\nSTATE: OFF \n");
        }
        printf("\nPERIOD: %d\n",recv_packet.period);
        printf("\nDUTYCYCLE: %d \n",recv_packet.dutycycle);

      }

      else{ 
        printf("\nWRONG COMMAND\n");
        continue;
      }


    }

  }
}
