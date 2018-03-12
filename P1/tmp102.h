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

#include "i2c_wrap.h"

#define TEMP_ADDR 0x48
#define TEMP_READ_REG 0x00
#define TEMP_CONFIG_REG 0x01
#define TEMP_TLOW_REG 0x10
#define TEMP_THIGH_REG 0x11
#define TEMP_THIGH_VAL 0x15
#define TEMP_TLOW_VAL 0x10

#define TEMP_CONFIG_VAL 0x60


typedef enum{
	CELCIUS,
	FARENHEIT,
	KELVIN
} unit_t;

float temp_conv(unit_t unit, char *buffer);	//conversion to deg celcius/ faranheit , kelvin
void temperature_read(int file_handler, char *buffer);
int initialize_temp();
void configreg_write(int file_handler, char *buffer);
void configreg_read(int file_handler, char *buffer);
void tlow_read(int file_handler, char *buffer);
void thigh_read(int file_handler, char *buffer);
void thigh_write(int file_handler, char *buffer);
void tlow_write(int file_handler, char *buffer);



