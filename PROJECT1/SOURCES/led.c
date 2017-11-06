#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "led.h"

void led_on(){
  FILE* LED_FILE=NULL;
  char* LED_PATH="/sys/class/leds/beaglebone:green:usr1/brightness";

  if(LED_FILE=fopen(LED_PATH,"r+")){
    fwrite("1",1,1,LED_FILE);
    fclose(LED_FILE);
  }
  else{
    printf("\nError opening file\n");
  }

}

void led_off(){
  FILE* LED_FILE=NULL;
  char* LED_PATH="/sys/class/leds/beaglebone:green:usr1/brightness";

  if(LED_FILE=fopen(LED_PATH,"r+")){
    fwrite("0",1,1,LED_FILE);
    fclose(LED_FILE);
  }
  else{
    printf("\nError opening file\n");
  }

}

