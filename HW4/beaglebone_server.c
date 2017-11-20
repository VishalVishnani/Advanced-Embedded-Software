/*INCLUDES*/
#include<stdbool.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>


/*Structure to send packet*/
typedef struct packet_t
{
  int led_state;
  int period;
  int dutycycle;
}packet;


int main(int argc, char *argv[])
{
  int sockfd, newsockfd, port_number, client_length;

  struct sockaddr_in server_address, client_address;

  int n = 0;

  if(argc < 2)
  {
    printf("\nERROR: PORT NUMBER\n");
    exit(1);
  }

  /*Create Socket*/
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0){ 
    printf("ERROR: SOCKET OPEN");
    exit(1);
  }

  bzero((char *)&server_address, sizeof(server_address));
  port_number = atoi(argv[1]);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port_number);

  /*BIND the socket to the port number*/
  if(bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
    printf("ERROR: BIND");
    exit(1);
  }

  listen(sockfd,5);
  client_length = sizeof(client_address);
  
  /*Accept the coonection from client*/
  newsockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_length);
  if(newsockfd < 0){
    printf("ERROR: ACCEPT");
    exit(1);
  }

  char command_query[10] = {0};

  
  int file = 0;

  /*Open the file to read or write*/
  file = open("/dev/simple_char_device", O_RDWR);
  if(file < 0)
  {
     perror("ERROR: OPENING CHAR DEVICE");
    return errno;
  }
  
  while(1){

    int ret = 0;
    printf("\nWAITING FOR COMMAND\n");

    recv(newsockfd,command_query,sizeof(command_query),0);
    if(strncmp(command_query,"COMMAND",strlen("COMMAND")) == 0){

      /*Structure to store the command packet received*/
      packet recv_packet;

  
      ret=read(newsockfd,&recv_packet,sizeof(recv_packet));
      printf("\nState = %d\n",recv_packet.led_state);
      printf("\nPeriod = %d\n",recv_packet.period);
      
      
      
      ret = write(file,&recv_packet,sizeof(recv_packet));
      if(ret< 0){
        perror("\nERROR: WRITE TO DEVICE FAILED\n");
        return errno;
      }

    }
    else if(strncmp(command_query,"QUERY",strlen("QUERY")) == 0)
    {
 
      /*Structure to read the device variable*/
      packet send_packet;

      ret = read(file,&send_packet,sizeof(send_packet));
      if(ret < 0){
        perror("\nERROR: READ TO DEVICE FAILED\n");
        return errno;
      }

      /*Send the device data to client*/
      write(newsockfd, &send_packet,sizeof(send_packet));
    }

  }

  close(file);
  return 0;
}
