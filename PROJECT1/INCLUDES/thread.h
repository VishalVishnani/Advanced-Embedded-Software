#ifndef THREAD_H
#define THREAD_H


#define NUM_THREADS (4)

#include "logger.h"
#include "queue.h"
#include <pthread.h>
#include <signal.h>
#include <string.h>

sig_atomic_t cond_type;
sig_atomic_t cond_type_temp;
sig_atomic_t cond_type_light;
sig_atomic_t destroy_all;
sig_atomic_t count_exit;
sig_atomic_t check_alive;
sig_atomic_t asynctemp_to_light;
sig_atomic_t asynclight_to_temp;
sig_atomic_t asyncmain_to_light;

uint8_t file_open[50];

pthread_t threads[NUM_THREADS];
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

void *func1(void* t);
void *func3(void* t);
void *func2(void* t);
void *func4(void* t);
void pthread_initialize(void);


#endif
