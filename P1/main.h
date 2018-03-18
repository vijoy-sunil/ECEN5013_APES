/***************************************************************
* AUTHOR  : Praveen Gnanaseakran
* DATE    : 03/08/2018
* DESCRITPTION  : main files
                  
* SOURCE FILES  : main.c
****************************************************************/

#include "includes.h"
#include "msgque.h"
#include "errorhandling.h"
#include "notify.h"
#include "threads.h"
#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define UNITERRUPTIBLE_SLEEP(t)                                                               \
  struct timespec current, remaining;                                          \
  current.tv_nsec = 0;                                                         \
  current.tv_sec = t;                                                          \
  do {                                                                         \
    ret = nanosleep(&current, &remaining);                                     \
    if (ret == -1) {                                                           \
      current.tv_sec = remaining.tv_sec;                                       \
      current.tv_nsec = remaining.tv_nsec;                                     \
    }                                                                          \
  } while (ret != 0);

sig_atomic_t light_heartbeat_flag;
sig_atomic_t temperature_heartbeat_flag;
sig_atomic_t logger_heartbeat_flag;
sig_atomic_t socket_heartbeat_flag;


void socket_heartbeat_handl(int sig) ;

void light_hearbeat_handl(int sig) ;

void temperature_heartbeat_handl(int sig) ;

void logger_heartbeat_handl(int sig) ;
