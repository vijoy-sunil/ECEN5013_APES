/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/08/2018
* DESCRITPTION	: 
		  
* HEADER FILES	: i2c_wrap.h
****************************************************************/

#include "i2c_wrap.h"

// file_handler -> temp_file or light_file
// check return codes when calling this function -> -1: fail

int i2c_init(char *path, int file_handler, int slave_addr)
{
	int rc;
	char buffer[1];

	pthread_mutex_init(&i2c_mutex, NULL);

	pthread_mutex_lock(&i2c_mutex);

	file_handler = open(path, O_RDWR);
	if (file_handler < 0){
		err(errno, "Tried to open '%s'", path); 
		file_handler = -1;
	}

	rc = ioctl(file_handler, I2C_SLAVE, slave_addr);
	if (rc < 0){
		err(errno, "Tried to set device address '0x%02x'", slave_addr);
		file_handler = -1;
	}

	//check if slave exists or not
	if(read(file_handler, buffer, 1) != 1)
		file_handler = -1;

	pthread_mutex_unlock(&i2c_mutex);
	return file_handler;
}

//Reference: https://www.kernel.org/doc/Documentation/i2c/dev-interface

// file_handler -> temp_file or light_file
// check return codes when calling this function -> 0: success -1: fail

int i2c_read(int file_handler, char *buffer, int num_bytes)
{
	int rt = 0;

	pthread_mutex_lock(&i2c_mutex);
	if( read(file_handler, buffer, num_bytes) != num_bytes){
		printf("READ ERROR: i2c transcation failed\n");
		rt = -1;
	}	
	pthread_mutex_unlock(&i2c_mutex);
	return rt;
}

// file_handler -> temp_file or light_file
// check return codes when calling this function -> 0: success. -1: fail

int i2c_write(int file_handler, char *buffer, int num_bytes)
{
	int rt = 0;
	pthread_mutex_lock(&i2c_mutex);
	if( write(file_handler, buffer, num_bytes) != num_bytes){
		printf("WRITE ERROR: i2c transcation failed\n");
		rt = -1;
	}
	pthread_mutex_unlock(&i2c_mutex);
	return rt;
		
}


