#ifndef messageQue_H
#define messageQue_H
#include <mqueue.h>


#define BUFFER_SIZE 4096
#define MESSAGEQ_SIZE 10
#define MSG_PRIO  30
#define MSG_PRIO_INIT 25
#define ERROR_MESSAGE_PRIORITY 20

#define LOGGER_MQ   "/logger_MQ"
#define MSGQ_ALERT   "/error_MQ6"
#define IPC_TEMP_MQ   "/IPC_TEMP_MQ"
#define IPC_LIGHT_MQ   "/IPC_LIGHT_MQ"

#endif
