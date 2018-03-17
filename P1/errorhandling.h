#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "packetdefinition.h"
#include "notify.h"
#include "queue.h"
#include <mqueue.h>

err_message_packet *message_packet;

void ErrorHandler(char* msg,mqd_t messagequeue_error,mqd_t message_queue_log,msg_type type);
struct sigevent event_error;
void ErrorNotify(union sigval sv);
