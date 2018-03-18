/***************************************************************
* AUTHOR  : Praveen Gnanaseakran
* DATE    : 03/08/2018
* DESCRITPTION  : SOCKET TASK
                  
* SOURCE FILES  : socketTask.c
****************************************************************/

#include "adps9301.h"
#include "tmp102.h"
#include "includes.h"
#include "msgque.h"
#include "errorhandling.h"
#include "notify.h"
#include "threads.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>