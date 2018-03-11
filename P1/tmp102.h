/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/08/2018
* DESCRITPTION	: 
		  
* SOURCE FILES	: tmp102.c
****************************************************************/

const int tmp102_addr	= 0x48;		//slave address
char *temp_path 	= "/dev/i2c-2";	//driver path
int temp_file;				//temp sensor file handler

typedef enum{
	TEMP, 
	CONFIG, 
	TLOW, 
	THIGH
}pointer_reg;

float temp_conv(unit_t unit, char *buffer);	//conversion to deg celcius/ faranheit , kelvin





