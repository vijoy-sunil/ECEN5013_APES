#ifndef messageQue_H
#define messageQue_H
#include <mqueue.h>


#define BUFFER_SIZE 4096
#define MESSAGEQ_SIZE 10
#define MSG_PRIO  30
#define MSG_PRIO_INIT 25
#define ERROR_MESSAGE_PRIORITY 20

#define LOGGER_MSGQ_IPC   "/logger_MQ"
#define ALERT_MSGQ_PCKT   "/error_MQ6"
#define TEMPERATURE_MSGQ_IPC   "/TEMPERATURE_MSGQ_IPC"
#define LIGHT_MSGQ_IPC   "/LIGHT_MSGQ_IPC"

#endif
