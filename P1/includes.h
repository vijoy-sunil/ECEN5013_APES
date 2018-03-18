#ifndef includes_H
#define includes_H

#include<stdint.h>
#include"messageQue.h"
#define PORT 8080
#define DEFAULT_FILE_NAME ("logfile.txt")
char* fileid;


#define BBB
//#define TEST

#ifdef BBB

#define LED_ON (system("echo 1 > /sys/class/leds/beaglebone:green:usr0/brightness"))
#define LED_OFF (system("echo 0 > /sys/class/leds/beaglebone:green:usr0/brightness"))
#define LED_OPTION(option) { if(option == 1) LED_ON; if(option == 0) LED_OFF; }

#endif

pthread_mutex_t light_lock;
pthread_mutex_t lock_i2c;

typedef enum{
	CELCIUS,
	FARENHEIT,
	KELVIN
} temp_unit;

typedef enum{
	LUMEN,
	DAY_NIGHT
} light_unit;

temp_unit temp_format;


typedef enum{
init,
error,
notification
}msg_type;

typedef struct{
char msg[BUFFER_SIZE-sizeof(msg_type)];
msg_type type;
}alert_message;

typedef enum {
  temperatue_Task,
  light_Task,
  RemoteRequestSocket_Task,
  error_handler
}task_type;


typedef struct{
char time_stamp[32];
task_type log_source;
char log_msg[BUFFER_SIZE -100];
uint8_t log_level;
}log_pack;

typedef enum{
temp,
light
}sensor_type;

//request structure
  typedef struct {
  sensor_type sensor;
  temp_unit tunit;//if sensor is temperature
  light_unit lunit;
}sock_req;

#endif
