/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/08/2018
* DESCRITPTION	: 
		  
* HEADER FILES	: tmp102.h
****************************************************************/

#include "tmp102.h"



// The Pointer Register uses the two least-significant bytes (LSBs) to identify
// which of the data registers must respond to a read or write command.
void tlow_read(int file_handler, char *buffer)
{
	char p1_p0= TEMP_TLOW_REG;
	
	i2c_write(file_handler, &p1_p0, 1);
	i2c_read(file_handler, buffer,2);
}

void thigh_read(int file_handler, char *buffer)
{
	char p1_p0= TEMP_THIGH_REG;
	
	i2c_write(file_handler, &p1_p0, 1);
	i2c_read(file_handler, buffer,2);
}

void tlow_write(int file_handler, char *buffer)
{
	buffer[0]= TEMP_TLOW_REG;
	buffer[1]= TEMP_TLOW_VAL;
	i2c_write(file_handler, buffer, 2);
}

void thigh_write(int file_handler, char *buffer)
{
	buffer[0]= TEMP_THIGH_REG;
	buffer[1]= TEMP_THIGH_VAL;
	i2c_write(file_handler, buffer, 2);
}

// Two bytes must be read to obtain data, byte 1 is the most significant byte, followed by byte 2, 
// the least significant byte. The first 12 bits are used to indicate temperature.
void temperature_read(int file_handler, char *buffer)
{
	char p1_p0= TEMP_READ_REG;
	
	i2c_write(file_handler, &p1_p0, 1);
	i2c_read(file_handler, buffer,2);
}

// returns -1 on fail
int initialize_temp()
{
	int temp;
	temp=i2c_init("/dev/i2c-2",temp,TEMP_ADDR);
	return temp;
}


// buffer -> 16 bits
// see macros in header file
void configreg_write(int file_handler, char *buffer)
{

	i2c_write(file_handler, buffer, 3);
	// i2c_write(file_handler, &buffer[1], 1);
}

void configreg_read(int file_handler, char *buffer )
{
	char p1_p0 = TEMP_CONFIG_REG;

	i2c_write(file_handler, &p1_p0, 1);
	i2c_read(file_handler, buffer,2);
}

// unit -> CELCIUS, FARANHEIT, KELVIN
// buffer -> buffer[0], buffer[1]
float temp_conv(unit_t unit, char *buffer)
{
	float temperature;
	unsigned char MSB, LSB;
	int temp_12b;

	// 12 bit result
	
	// Reference: http://bildr.org/2011/01/tmp102-arduino
	MSB = buffer[0];
	LSB = buffer[1];
	temp_12b = ((MSB << 8) | LSB) >> 4;

	int is_neg;
	// 0 positive, 1 negative
	is_neg = temp_12b & 0x800;

	if(is_neg == 1)
	{
		temp_12b = ~temp_12b + 1; 
	}
	
	switch(unit)
	{
		case CELCIUS:
			temperature = temp_12b * 0.0625;
			if(is_neg == 1)
				temperature = -temperature;
			break;

		case FARENHEIT:
			if(is_neg == 1)
				temperature = -(temp_12b * 0.0625);
			else
				temperature = (temp_12b * 0.0625);
			temperature = 1.8 * temperature + 32;
			break;

		case KELVIN:
			if(is_neg == 1)
				temperature = -(temp_12b * 0.0625);
			else
				temperature = (temp_12b * 0.0625);
			temperature = 273.15 + (temperature);
			break;

		default:
			printf("INVALID conversion unit\n");
			temperature = 0;
			break;
	}
	return temperature;
}


