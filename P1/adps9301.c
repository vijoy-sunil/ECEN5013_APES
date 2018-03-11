/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/08/2018
* DESCRITPTION	: 
		  
* HEADER FILES	: adps9301.h
****************************************************************/

#include "adps9301.h"

int initialize_light(void)
{
	int light;
	temp = i2c_init("/dev/i2c-2", light, LIGHT_ADDR);
	return light;
}

// op-> READ or WRITE
// data is 8 bits
void command_reg(int file_handler, adps9301_regs_t adps9301_reg, op_t op)
{
	char CMD, CLEAR, WORD, RES;

	CMD = 1;
	CLEAR = 1;
	WORD = op;	// read or write
	RES = 0;

	char data = CMD << 7 | CLEAR << 6 | WORD << 5 | RES << 4 | adps9301_reg;
	i2c_write(file_handler, &data , 1);
}

// op1-> READ or WRITE
// op2-> ENABLE or DISABLE
// buffer is 8 bit
void control_reg(int file_handler, op_t op1, op_t op2, char *buffer)
{
	char data;
	if(op1 == READ)
		i2c_read(file_handler, buffer , 1);
	else if(op1 == WRITE)
	{
		if(op2 == ENABLE){
			data = POWERUP;
			i2c_write(file_handler, &data , 1);
		}
		else if(op2 == DISABLE){
			data = POWERDOWN;
			i2c_write(file_handler, &data , 1);
		}	
	}

	else
		printf("ERROR: invalid operation\n");
}

// op1-> READ or WRITE
// op2-> SET_GAIN or SET_INTEGTIME
// op3-> LOW_GAIN, HIGH_GAIN | INTEG0 or INTEG1 or INTEG2 or INTEG3
// buffer is 8 bits
void timing_reg(int file_handler, op_t op1, op_t op2, op_t op3, char *buffer )
{
	char data, GAIN, INTEG;
	if(op1 == WRITE)
	{
		data = 0x00;
		if(op2 == SET_GAIN)
		{
			if(op3 == LOW_GAIN){
				GAIN = 0;
				data |=  GAIN <<3;
				i2c_write(file_handler, &data , 1);
			}
			else if(op3 == HIGH_GAIN){
				GAIN = 1;
				data |= GAIN << 3;
				i2c_write(file_handler, &data , 1);
			}
		}

		else if(op2 == SET_INTEGTIME)
		{
			switch(op3)
			{
				case INTEG0:
					INTEG = 0;
					data |= INTEG;
					break;

				case INTEG1:
					INTEG = 1;
					data |= INTEG;
					break;

				case INTEG2:
					INTEG = 2;
					data |= INTEG;
					break;

				case INTEG3:
					INTEG = 3;
					data |= INTEG;
					break;
			}
			i2c_write(file_handler, &data , 1);
		}
	}

	else if(op1 == READ)
	{
		i2c_read(file_handler, buffer , 1);
	}

	else
		printf("ERROR: invalid operation\n");

}

// op-> ENABLE or DISABLE
void interrup_reg(int file_handler, op_t op)
{
	char data;
	if(op == ENABLE){
		data = 0x10;
		i2c_write(file_handler, &data , 1);
	}	
	else if(op == DISABLE){
		data = 0x00;
		i2c_write(file_handler, &data , 1);
	}	
	else
		printf("ERROR: invalid operation\n");
	
}

void id_reg_read(int file_handler)
{
	char part_num, rev_num, id;
	i2c_read(file_handler, &id , 1);

	part_num = id >> 4;
	rev_num = 0x0F & id;

	printf("PART NUMBER: %d\n", part_num);
	printf("REV NUMBER: %d\n", rev_num);	
}

// op-> READ or WRITE
void interrupt_thresh_reg(int file_handler, op_t op, char *buffer)
{
	if(op == READ)
		i2c_read(file_handler, buffer , 1);
	else if(op == WRITE)
		i2c_write(file_handler, buffer, 1);
	else
		printf("ERROR: invalid operation\n");
}


// buffer-> buffer[0](lsb) and buffer[1](msb)
// channel
uint16_t adc_data_read(int file_handler, int channel)
{
	char buffer[2] ={0};
	int adc_data;

	if(channel == 0){
		command_reg(file_handler, DATA0LOW, READ);
		i2c_read(file_handler, &buffer[0] , 1);

		command_reg(file_handler, DATA0HIGH, READ);
		i2c_read(file_handler, &buffer[1] , 1);
	}

	else if (channel == 1){
		command_reg(file_handler, DATA1LOW, READ);
		i2c_read(file_handler, &buffer[0] , 1);

		command_reg(file_handler, DATA1HIGH, READ);
		i2c_read(file_handler, &buffer[1] , 1);
	}

	else
		printf("ERROR: invalid operation\n");

	
	adc_data = (buffer[1] << 8 | buffer[0]);
	return adc_data;
}

float report_lumen(uint16_t adc_data_ch0, uint16_t adc_data_ch1)
{
	float lumen, ratio;
	ratio = adc_data_ch1/adc_data_ch0;

	if(ratio > 0 && ratio <= 0.50)
		lumen = (0.0304 * adc_data_ch0) - (0.062 * adc_data_ch0 * (ratio)^);
	else if(ratio > 0.50 && ratio <= 0.61)
		lumen = (0.0224 * adc_data_ch0) - (0.031 * adc_data_ch1);
	else if(ratio > 0.61 && ratio <= 0.80)
		lumen = (0.0128 * adc_data_ch0) - (0.0153 * adc_data_ch1);
	else if(ratio > 0.80 && ratio <= 1.30)
		lumen = (0.00146 * adc_data_ch0) - (0.00112 * adc_data_ch1);
	else
		lumen = 0;
	
	
	return lumen;
}

tod_t report_tod(int file_handler)
{
	tod_t tod;
	int adc_ch0, adc_ch1;
	adc_ch0 = adc_data_read(file_handler, 0);
	adc_ch1 = adc_data_read(file_handler, 1);

	float lumen = repot_lumen(adc_ch0, adc_ch1);

	if(lumen >= lum_DAY)
		tod = DAY;
	else
		tod = NIGHT;
	return tod;
}




















