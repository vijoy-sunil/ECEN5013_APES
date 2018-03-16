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
	light = i2c_init("/dev/i2c-2", light, LIGHT_ADDR);
	return light;
}

// op-> READ or WRITE
// data is 8 bits
void command_reg(int file_handler, adps9301_regs_t adps9301_reg, op_t op)
{
	char CMD, CLEAR, WORDS, RES;

	CMD = 1;
	CLEAR = 0;
	WORDS = 0;	// read or write
	RES = 0;

	char data = CMD << 7 | CLEAR << 6 | WORDS << 5 | RES << 4 | adps9301_reg;
	//printf("COMAMDNREG %x\n",data);
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
	char GAIN, INTEG;
	static char data = 0x00;
	if(op1 == WRITE)
	{
		if(op2 == SET_GAIN)
		{
			if(op3 == LOW_GAIN){
				GAIN = 0;
				data |=  GAIN <<4;
				i2c_write(file_handler, &data , 1);
			}
			else if(op3 == HIGH_GAIN){
				GAIN = 1;
				data |= GAIN << 4;
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
	char buffer[2];
	// buffer[0]= DATA0LOW|0x80;
	//buffer[0]=0x8c;
	uint16_t adc_data;

	if(channel == 0){
		command_reg(file_handler, DATA0LOW, WRITE);
		i2c_read(file_handler, &buffer[0] , 1);

		command_reg(file_handler, DATA0HIGH, WRITE);
		i2c_read(file_handler, &buffer[1] , 1);
	}

	else if (channel == 1){
		command_reg(file_handler, DATA1LOW, WRITE);
		i2c_read(file_handler, &buffer[0] , 1);s

		command_reg(file_handler, DATA1HIGH, WRITE);
		i2c_read(file_handler, &buffer[1] , 1);
	}

	else
		printf("ERROR: invalid operation\n");

	//printf("BUFFER %x %x\n",buffer[1],buffer[0]);
	adc_data = (buffer[1] << 8 | buffer[0]);
	return adc_data;
}

float report_lumen(uint16_t adc_data_ch0, uint16_t adc_data_ch1)
{
	float lumen, ratio;
	
	ratio = (float)adc_data_ch1/adc_data_ch0;
	printf("ch0 %d, ch1 %d\n", adc_data_ch0, adc_data_ch1);

	if(ratio > 0 && ratio <= 0.50)
	{
		double power=pow(ratio,1.4);
		lumen = (0.0304 * adc_data_ch0) - (0.062 * adc_data_ch0 * power);
	}
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
	uint16_t adc_ch0, adc_ch1;
	adc_ch0 = adc_data_read(file_handler, 0);
	adc_ch1 = adc_data_read(file_handler, 1);

	float lumen = report_lumen(adc_ch0, adc_ch1);

	if(lumen <= lum_NIGHT)
		tod = NIGHT;
	else
		tod = DAY;
	return tod;
}




















