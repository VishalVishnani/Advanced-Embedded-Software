/*****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : thread.h
* Description : Header file for Threads
*               -func1()
*               -func2()
*               -func3()
*               -func4()
*               -pthread_initialize()
******************************************************************************************/



#ifndef THREAD_H
#define THREAD_H


#define NUM_THREADS (4)

/*INCLUDES*/
#include "logger.h"
#include "queue.h"
#include <pthread.h>
#include <signal.h>
#include <string.h>


/*Atomic Variables used to signal events and for synchronization*/
sig_atomic_t cond_type;
sig_atomic_t cond_type_temp;
sig_atomic_t cond_type_light;
sig_atomic_t destroy_all;
sig_atomic_t count_exit;
sig_atomic_t check_alive;
sig_atomic_t asyncmain_to_light;

uint8_t file_open[50];

/*Threads*/
pthread_t threads[NUM_THREADS];

/*Mutex and Condition Variables*/
pthread_mutex_t temp_w_lock;
pthread_mutex_t light_w_lock;
pthread_cond_t timer_expire_temp;
pthread_cond_t timer_expire_light;
pthread_mutex_t main_queue_lock;
pthread_mutex_t decision_lock;


pthread_mutex_t heart_beat_temp_lock;
pthread_mutex_t heart_beat_light_lock;
pthread_mutex_t heart_beat_logger_lock;
pthread_mutex_t heart_beat_decision_lock;

pthread_cond_t heartbeat_temp;
pthread_cond_t heartbeat_light;
pthread_cond_t heartbeat_logger;
pthread_cond_t heartbeat_decision;


/******************************************************************************************
*
* func1() - Temperature thread which takes the temperature value from the sensor at
*           periodic intervals,stores it in Temperature Queue and also receives 
*           asynchronous requests from other threads in another queues and services 
*           that request. It also periodically updates main that it is working
********************************************************************************************/
void *func1(void* t);



/******************************************************************************************
*
* func2() - Light thread which takes the temperature value from the sensor at
*           periodic intervals,stores it in Light Queue and also receives 
*           asynchronous requests from other threads in another queue and services 
*           that requests. It also periodically updates main that it is working
********************************************************************************************/
void *func2(void* t);


/******************************************************************************************
*
* func3() - Logger thread which reads from temperature and light queue and logs it
*           in LOG file. It also periodically updates main that it is working
********************************************************************************************/
void *func3(void* t);


/******************************************************************************************
*
* func4() - Decisiom thread which reads from queue and takes decision based on it (ALERTS). 
*           It also periodically updates main that it is working
********************************************************************************************/
void *func4(void* t);


/*Function to initialize pthreads, mutexes and condition variables*/
void pthread_initialize(void);


#endif
