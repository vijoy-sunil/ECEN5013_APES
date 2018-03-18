/***************************************************************
* AUTHOR  : Vijoy Sunil Kumar
* DATE    : 03/08/2018
* DESCRITPTION  : Logger Sensor Task
                  
* Source FILES  : LoggerTask.h
****************************************************************/

#include "includes.h"
#include "msgque.h"
#include "errorhandling.h"
#include "notify.h"
#include "threads.h"
#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>