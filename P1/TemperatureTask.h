#include "tmp102.h"
#include "includes.h"
#include "msgque.h"
/***************************************************************
* AUTHOR  : Vijoy Sunil Kumar
* DATE    : 03/08/2018
* DESCRITPTION  : Temperature Sensor Task
                  
* SOURCE FILES  : TemperatureTask.c
****************************************************************/

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

void IPCTemperature_Handl(int sig);