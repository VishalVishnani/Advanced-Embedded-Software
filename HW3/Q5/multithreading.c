/***************************************************************************
* Author: Vishal Vishnani
* Date: 10/5/2017
* File: multithreading.c
* Description: Source file for Multithreaded Reading and Writing to a file
***************************************************************************/

//Includes
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>


//Macros
#define BUFFER_SIZE 2048
#define WORD_NO 200
#define WORD_SIZE 20


//Mutex and condition variables
pthread_mutex_t file_access;
pthread_cond_t user_sig1;
pthread_cond_t disp_sig;

//Global variables shared among threads
int8_t filename[10];
long read_at=0;
long write_at=0;
FILE* file;
int8_t words[WORD_NO][WORD_SIZE];
uint16_t index_n[WORD_NO];
uint16_t count[WORD_NO];
uint8_t tflag=0;
uint16_t word_no=0;

//Structure to store file statistics
typedef struct statistics{
  volatile uint16_t charcount;
  volatile uint16_t wordcount;
  volatile uint16_t linecount;
}stat;

stat* stat1;


//Function which finds the occurences of each word in a file
void map(void){
  int8_t string[WORD_SIZE];
  uint16_t i=0,j=0;
  long f_size=0;
  word_no=0;
  uint8_t cmp=0;
  uint8_t cmp_found=0;
  int8_t string2[WORD_SIZE];
  uint16_t temp=0;
  uint16_t temp_index=0;
  int8_t ret;

  //Clear words,count and index_n array function everytime the map function is called
  for(i=0;i<WORD_NO;i++){
    memset(words[i],0,WORD_SIZE);
  }
  memset(count,0,sizeof(count));
  memset(index_n,0,sizeof(index_n));
  
  //Set the file pointer to starting position
  ret=fseek(file,0,SEEK_SET);
  if(ret<0)
    printf("\nFseek failed\n");

  //Read every word of the file
  while(fscanf(file,"%s",string)==1){

    //check if the word already exists in the words array
    //If yes, then increment the count for that word in count array
    //If no, store that word in the words array
    for(i=0;i<word_no;i++){
     
      strcpy(string2,words[i]);
   
      cmp=strcasecmp(string,string2);
      if(cmp==0){
        count[i]=count[i]+1;
        cmp_found=1;
      }
      if(cmp_found==1)
        break;
    }
      if(cmp_found==0){
        strcpy(words[word_no],string);
        word_no++;
      }
      cmp_found=0;
  }

  //Intialize the index array
  for(i=0;i<word_no;i++){
    index_n[i]=i;
  }

  //Swap the count array in descending order and store their previous position in the index_n array
  for(i=0;i<word_no;++i){
    for(j=i+1;j<word_no;++j){
      if(count[i]<count[j]){
        temp=count[i];
        temp_index=index_n[i];
        count[i]=count[j];
        index_n[i]=index_n[j];
        count[j]=temp;
        index_n[j]=temp_index;
      }
    }
  }
}



//Signalhandler for SIGINT
void siginthandler(int sig_num){
  printf("Inside SIGINT handler function\n");

  int8_t ret;
  tflag=1;


  ret=pthread_cond_signal(&user_sig1);
  if(ret<0)
    printf("\nPthread_cond_signal(&user_sig1) failed\n");


  pthread_cond_signal(&disp_sig);
  if(ret<0)
    printf("\nPthread_cond_signal(&disp_sig) failed\n");



  //Destroy mutex and condition variables                      
  ret=pthread_mutex_destroy(&file_access);
  if(ret<0)
    printf("\nPthread_mutex_destroy(&file_access) failed\n");

  ret=pthread_cond_destroy(&disp_sig);
  if(ret<0)
    printf("\nPthread_cond_destroy(&disp_sig) failed\n");

  

  ret=pthread_cond_destroy(&user_sig1);
  if(ret<0)
    printf("\nPthread_cond_destroy(&user_sig1) failed\n");


  //free the memory allocated to the structure
  free(stat1);

  //close the file
  fclose(file);
  exit(1);
}


//Signalhandler for SIGUSR1
void sighandler(int signum){

  int8_t ret;
  printf("\nIn SIGUSR1 Handler\n");

  //store the current file pointer position
  write_at=ftell(file); 
 
  //send user_sig1 signal for which child thread1 is waiting
  ret=pthread_cond_signal(&user_sig1);
  if(ret<0)
    printf("\nPthread_cond_signal(&user_sig1) failed\n");


  sleep(1);
}


//Signalhandler for SIGUSR2
void sighandler1(int signum){

  int8_t ret;
  printf("\nIn SIGUSR2 Handler1\n");

  //send disp_sig for which child thread2 is waiting  
  ret=pthread_cond_signal(&disp_sig);
  if(ret<0)
    printf("\nPthread_cond_signal(&disp_sig) failed\n");


  sleep(1);
}


//Function for child thread1
void* process_func(void* a){
  int8_t ret;
  while(1){

    //Lock the mutex as we will be accessing global variables and reading the file
    ret=pthread_mutex_lock(&file_access);
    if(ret<0)
      printf("\nPthread_mutex_lock(&file access) failed in process_func\n");


    //Wait for user_sig1 signal which is received from SIGUSR1
    ret=pthread_cond_wait(&user_sig1,&file_access);
    if(ret<0)
      printf("\nPthread_cond_wait() failed in process_func\n");


    if(tflag==1){
      ret=pthread_mutex_unlock(&file_access);
      if(ret<0)
        printf("\nPthread_mutex_unlock(&file access) failed in process_func\n");
      break;
    }
    ret=fseek(file,0,SEEK_SET);
    if(ret<0)
      printf("\nfseek failed failed in process_func\n");


    //set the structure values to 0
    stat1->charcount=0;
    stat1->wordcount=0;
    stat1->linecount=0;

    char ch;
 

    //Read file until the write_at position which is position of the last character
    //Calculate the words, lines, characters
    while(ftell(file)!=write_at)
    {
      ch=fgetc(file);
      stat1->charcount++;
      if(ch==' ')
        stat1->wordcount++;
      if(ch=='\n')
      {
        stat1->linecount++;
        stat1->wordcount++;
      }
    }
 
    //calculate the occurences of each word
    map();

    //update the read_at position to the current position
    read_at=ftell(file);

    //Unlock the mutex
    ret=pthread_mutex_unlock(&file_access);
    if(ret<0)
      printf("\nPthread_mutex_unlock(&file access) failed in process_func\n");

  }

  printf("\nExit process function\n");
  pthread_exit(NULL);

}


//Function executed by the child thread2
void* display_func(void* a){
  int8_t ret;
  while(1){

    //Lock the mutex as we will display the value of global variables
    //we do not want them to be changed while we display, hence mutex lock
    ret=pthread_mutex_lock(&file_access);
    if(ret<0)
      printf("\nPthread_mutex_lock(&file access) failed in display_func\n");


    //wait for disp_sig signal from SIGUSR2
    ret=pthread_cond_wait(&disp_sig,&file_access);
    if(ret<0)
      printf("\nPthread_cond_wait() failed in display_func\n");


    if(tflag==1){
      ret=pthread_mutex_unlock(&file_access);
      if(ret<0)
        printf("\nPthread_mutex_unlock() failed in display_func\n");

      break;
    }
  
    printf("\ntotal no of character=%d",stat1->charcount);
    printf("\ntotal no of words=%d",stat1->wordcount);
    printf("\ntotal no of lines=%d\n",stat1->linecount);

    printf("\nTOP WORDS IN FILE\n");
    uint8_t i;
    uint8_t j;

    if(word_no<5)
      j=word_no;
    else
      j=5;

    for(i=0;i<j;i++){
      printf("Word: %s  Occurences: %d\n",words[index_n[i]],count[i]+1);
    }
    
    //Unlock the mutex
    ret=pthread_mutex_unlock(&file_access);
    if(ret<0)
      printf("\nPthread_cond_wait() failed in display_func\n");

  }

  printf("\nExiting display function\n");
  pthread_exit(NULL);
}


int main()
{
 
  int8_t buffer[BUFFER_SIZE];
  int8_t ret;

  //Initialize mutex and condition variables
  ret=pthread_mutex_init(&file_access,NULL);
  if(ret<0)
      printf("\nPthread_mutex_init() failed in main\n");


  pthread_cond_init(&user_sig1,NULL);
  if(ret<0)
      printf("\nPthread_cond_init() failed in main\n");


  //Initialize signal and signal handlers
  signal(SIGUSR1,sighandler);
  signal(SIGUSR2,sighandler1);
  signal(SIGINT,siginthandler);

  //dynamic memory allocation for the stats structure
  stat1=(stat*)malloc(sizeof(stat));

  //Clear the buffer and file name
  memset(filename,0,10);
  memset(buffer,0,BUFFER_SIZE);

  //Get filename from user
  printf("\nEnter the Filename to write\n");
  scanf("%s%*c",filename);
  printf("Filename: %s\n",filename);
  
  //Open file
  file=fopen(filename,"w+");
  if (file==NULL){
    printf("Error opening file\n");
    exit(1);
  }

  //Initial file pointer position is 0
  read_at=ftell(file);
 // printf("Read at in main: %ld\n",read_at);
  
  //Pthreads
  pthread_t thread1,thread2;
  int rc;

  //Create child thread1 for process_func
  rc=pthread_create(&thread1,NULL,process_func,NULL);
  if(rc)
    printf("error in thread1 create\n");

  //Create child thread2 for display_func
  rc=pthread_create(&thread2,NULL,display_func,NULL);
  if(rc)
    printf("error in thread2 create\n");



  while(1){

    //Get the data to write from user
    printf("\nEnter the data to write\n");

    

    fgets(buffer,BUFFER_SIZE,stdin);
  
    //Calculate the size of data written
    uint8_t i=0;
    while(strcmp((buffer+i),"\0")!=0 && i < BUFFER_SIZE){
      i++;      
    }
     
    //Lock the mutex before writing to the file
    ret=pthread_mutex_lock(&file_access);
    if(ret<0)
      printf("\nPthread_mutex_lock() failed in main\n");

   
    fwrite(buffer,1,i,file);

    //Unlock mutex after writing to the file
    ret=pthread_mutex_unlock(&file_access);
    if(ret<0)
      printf("\nPthread_mutex_unlock() failed in main\n");


  }

  return 0;

}

