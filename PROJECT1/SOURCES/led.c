/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : led.h
* Description : Source file for LEDS
*               -led_on()
*               -led_off()
******************************************************************************************/



#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "led.h"

/*Function to turn on leds*/
void led_on(){
  FILE* LED_FILE=NULL;
  char* LED_PATH="/sys/class/leds/beaglebone:green:usr1/brightness";

  if(LED_FILE=fopen(LED_PATH,"r+")){
    /*Write 1 to turn on*/
    fwrite("1",1,1,LED_FILE);
    fclose(LED_FILE);
  }
  else{
    printf("\nError opening file\n");
  }

}

/*function to turn off leds*/
void led_off(){
  FILE* LED_FILE=NULL;
  char* LED_PATH="/sys/class/leds/beaglebone:green:usr1/brightness";

  if(LED_FILE=fopen(LED_PATH,"r+")){
    /*Write 0 to turn off*/
    fwrite("0",1,1,LED_FILE);
    fclose(LED_FILE);
  }
  else{
    printf("\nError opening file\n");
  }

}

