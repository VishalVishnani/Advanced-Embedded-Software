#ifndef TIMER_H
#define TIMER_H

#include <signal.h>
#include <sys/time.h>
#include <string.h>

struct sigaction sa;
struct itimerval timer;


void timer_init(void);
struct timespec heartbeat_init(uint32_t sec_value,uint32_t nsec_value);

#endif
