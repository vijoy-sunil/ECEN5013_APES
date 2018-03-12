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
	file_handler = open(path, O_RDWR);
	if (file_handler < 0){
		err(errno, "Tried to open '%s'", path); 
		return -1;
	}

	rc = ioctl(file_handler, I2C_SLAVE, slave_addr);
	if (rc < 0){
		err(errno, "Tried to set device address '0x%02x'", slave_addr);
		return -1;
	}

	//check if slave exists or not
	if(read(file_handler, buffer, 1) != 1)
		return -1;
	return file_handler;
}

//Reference: https://www.kernel.org/doc/Documentation/i2c/dev-interface

// file_handler -> temp_file or light_file
// check return codes when calling this function -> 0: success -1: fail

int i2c_read(int file_handler, char *buffer, int num_bytes)
{
	if( read(file_handler, buffer, num_bytes) != num_bytes){
		printf("READ ERROR: i2c transcation failed\n");
		return -1;
	}	
	return 0;
}

// file_handler -> temp_file or light_file
// check return codes when calling this function -> 0: success. -1: fail

int i2c_write(int file_handler, char *buffer, int num_bytes)
{
	if( write(file_handler, buffer, num_bytes) != num_bytes){
		printf("WRITE ERROR: i2c transcation failed\n");
		return -1;
	}

	return 0;
		
}


