#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "tasks.h"
#include "notify.h"
#include "queue.h"
#include <mqueue.h>
#include "packetdefinition.h"
#include "errorhandling.h"

#include "adps9301.h"

#define NSEC_FREQUENCY (1000000000)