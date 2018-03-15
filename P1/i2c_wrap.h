/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/08/2018
* DESCRITPTION	: 
		  
* SOURCE FILES	: i2c_wrap.c
****************************************************************/

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

#include <pthread.h>

int i2c_init(char *path, int file_handler, int slave_addr);
int i2c_read(int file_handler, char *buffer, int num_bytes);
int i2c_write(int file_handler, char *buffer, int num_bytes);

pthread_mutex_t i2c_mutex;
