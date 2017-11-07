/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : led.h
* Description : Header file for LEDS
*               -led_on()
*               -led_off()
******************************************************************************************/




#ifndef LED_H
#define LED_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/*Function to turn led on*/
void led_on();

/*function to turn led off*/
void led_off();

#endif
