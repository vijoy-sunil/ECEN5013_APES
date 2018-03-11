/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/08/2018
* DESCRITPTION	: 
		  
* SOURCE FILES	: tmp102.c
****************************************************************/
// #include "i2c_wrap.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// const int tmp102_addr	= 0x48;		//slave address
// char *temp_path 	= "/dev/i2c-2";	//driver path
int temp_file;				//temp sensor file handler

typedef enum{
	TEMP, 
	CONFIG, 
	TLOW, 
	THIGH
}pointer_reg;


typedef enum{
	CELCIUS,
	FARENHEIT,
	KELVIN
} unit_t;
float temp_conv(unit_t unit, char *buffer);	//conversion to deg celcius/ faranheit , kelvin

void temperature_read(int file_handler, char *buffer);

int initialize_temp();





