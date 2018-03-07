/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/02/2018
* DESCRITPTION	: This program demostrates different IPC mechanisms
		  that is used to transfer a message structure
		  between two processes which is also used to control 
		  user LED0 on BBB board.
		 
* SOURCE FILES	: ipc_demo.c
****************************************************************/



#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/un.h>

#define BUFFER_DEPTH 4096

typedef struct 
{
	uint8_t led_control;
        char msg[BUFFER_DEPTH - sizeof(uint8_t)];	
}packet;

#define PORT 8080
//Reference: https://elinux.org/EBC_Exercise_10_Flashing_an_LED
#define READY_LED 	(system("echo none >/sys/class/leds/beaglebone:green:usr0/trigger"))
#define ON 		(system("echo 1 > /sys/class/leds/beaglebone:green:usr0/brightness"))
#define OFF 		(system("echo 0 > /sys/class/leds/beaglebone:green:usr0/brightness"))

#define SET_LED(state) {if(state == 1) ON; \
                         if(state == 0) OFF; }


#define MSQ "/send_receive_mq"
#define SOCK_PATH "echo_socket"


