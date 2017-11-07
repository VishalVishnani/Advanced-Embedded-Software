/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : timer.h
* Description : Header file for Timer
*               -timer_init()
*               -timespec_heartbeat_init()
******************************************************************************************/



#ifndef TIMER_H
#define TIMER_H

/*INCLUDES*/
#include <signal.h>
#include <sys/time.h>
#include <string.h>

/*Structures used*/
struct sigaction sa;
struct itimerval timer;

/*Atomic variable to increase the timer period*/
sig_atomic_t temp_timer_count;
sig_atomic_t light_timer_count;


/******************************************************************************************
* timer_init() - Function to initialize timer
*           
*******************************************************************************************/
void timer_init(void);

/******************************************************************************************
* timespec_heartbeat_init() - Function to initialize heartbeat timer
*           
*
* uint32_t sec_value - Seconds value
* uint32_t slave_address - Nanoseconds value
*     
* struct timespec(return)- Returns the timer structure
*******************************************************************************************/

struct timespec heartbeat_init(uint32_t sec_value,uint32_t nsec_value);


#endif
