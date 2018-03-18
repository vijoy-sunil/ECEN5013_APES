#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "i2c_wrap.h"
#include "includes.h"

#define TEMP_ADDR 			(0x48)
#define TEMP_READ_REG 	(0x00)
#define TEMP_CONFIG_REG (0x01)
#define TEMP_TLOW_REG 	(0x02)
#define TEMP_THIGH_REG 	(0x03)

#define TEMP_THIGH_VAL_B1 	(0x20)
#define TEMP_TLOW_VAL_B1 	(0x19)

#define TEMP_THIGH_VAL_B2 	(0x50)
#define TEMP_TLOW_VAL_B2	(0x50)

#define TEMP_CONFIG_VAL (0x60)


#define SHUTDOWN_EN 	0x01
#define SHUTDOWN_DI	0xFE
#define TM_EN		0x02
#define TM_DI		0xFD
#define FAULT1		0x00
#define FAULT2		0x08
#define FAULT4		0x10
#define FAULT6		0x18

//2nd byte
#define EMMODE_EN	0x10
#define EMMODE_DI       0xEF

#define CONVRATE0	0x00
#define CONVRATE1	0x40
#define CONVRATE2	0x80
#define CONVRATE3	0xC0


float covert_temperature(temp_unit unit, char *buffer);

void temperatureRead(int i2c_file_handler, char *buffer);

int Temp_sensor_init();

void temp_CONFIG_write(int i2c_file_handler, char *buffer);

void temp_CONFIG_read(int i2c_file_handler, char *buffer);
void tlowRead(int i2c_file_handler, char *buffer);
void temp_THIGH_read(int i2c_file_handler, char *buffer);

void temp_THIGH_write(int i2c_file_handler);
void temp_TLOW_write(int i2c_file_handler);
