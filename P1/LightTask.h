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
#include <mqueue.h>
#include "includes.h"
#include "errorhandling.h"
#include "adps9301.h"

void Handl_LightMsg(int sig);