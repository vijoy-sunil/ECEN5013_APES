#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include "includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
// #include "threads.h"
#include "notify.h"
#include "msgque.h"
#include <mqueue.h>

alert_message *message_packet;

struct sigevent signal_event;

/**
*@brief:
*this thread is created for mq_notify event on error message que
*@param:
*@return:
*/
void alertReceive(union sigval sv);

/**
*@brief:
*
*@param:
*@return:
*/
void alert(char* msg,mqd_t alertmsg_queue,mqd_t msgq_log,msg_type type);

#endif
