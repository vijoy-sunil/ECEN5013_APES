
#ifndef packetdefinition_H
#define packetdefinition_H

#include <stdint.h>
#include "queue.h"

typedef enum {
  TempTask,
  LightTask,
  RemoteTask,
  ErrorTask
}task_type;

typedef enum{
init,
error
}msg_type;

typedef struct{
char msg[BUFFER_SIZE-sizeof(msg_type)];
msg_type type;
}err_message_packet;

typedef struct{
char time_stamp[32];
uint8_t log_level;
task_type log_source;
char log_msg[16];
}log_pack;



#endif