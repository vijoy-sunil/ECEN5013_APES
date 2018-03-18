#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "threads.h"
#include "notify.h"
#include "msgque.h"
/***************************************************************
* AUTHOR  : Vijoy Sunil Kumar
* DATE    : 03/08/2018
* DESCRITPTION  : Light Sensor Task
                  
* SOURCE FILES  : LightTask.c
****************************************************************/

#include <mqueue.h>
#include "includes.h"
#include "errorhandling.h"
#include "adps9301.h"

void Handl_LightMsg(int sig);