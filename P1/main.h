#include "includes.h"
#include "threads.h"
#include "signals.h"
#include "messageQue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "errorhandling.h"

sig_atomic_t light_heartbeat_flag;
sig_atomic_t temperature_heartbeat_flag;
sig_atomic_t logger_heartbeat_flag;

#define SLEEP(t)      struct timespec current,remaining; \
        current.tv_nsec = 0; current.tv_sec = t; \
        do { \
                rc = nanosleep(&current,&remaining); \
                if(rc == -1) { \
                        current.tv_sec = remaining.tv_sec; \
                        current.tv_nsec = remaining.tv_nsec;} \
        } while(rc != 0);  \

