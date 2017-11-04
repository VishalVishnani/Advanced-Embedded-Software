#ifndef THREAD_H
#define THREAD_H


#define NUM_THREADS (3)

#include "logger.h"
#include "queue.h"
#include <pthread.h>
#include <signal.h>
#include <string.h>

sig_atomic_t cond_type;
sig_atomic_t destroy_all;
sig_atomic_t count_exit;

uint8_t file_open[50];

pthread_t threads[NUM_THREADS];
pthread_mutex_t temp_w_lock;
pthread_mutex_t light_w_lock;
pthread_cond_t timer_expire;
pthread_mutex_t main_queue_lock;

void *func1(void* t);
void *func3(void* t);
void *func2(void* t);
void pthread_initialize(void);


#endif
