#include "tmp102.h"
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
#include <string.h>
#include <time.h>
#include <unistd.h>

#define GET_TIME                                                               \
  clock_gettime(CLOCK_MONOTONIC, &current);                                        \
  expire.tv_sec = current.tv_sec + 2;                                              \
  expire.tv_nsec = current.tv_nsec;
