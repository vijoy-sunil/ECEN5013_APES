/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/08/2018
* DESCRITPTION	: 
		  
* SOURCE FILES	: adps9301.c
****************************************************************/

#include "i2c_wrap.h"
#include <math.h>

//slave address
#define LIGHT_ADDR	0x39

//sensor registers
typedef enum{
	CONTROL,
	TIMING,
	THRESHLOWLOW,
	THRESHLOWHIGH,
	THRESHHIGHLOW,
	THRESHHIGHIGH,
	INTERRUPT,
	RES0,
	CRC,
	RES1,
	ID,
	RES2,
	DATA0LOW,
	DATA0HIGH,
	DATA1LOW,
	DATA1HIGH
}adps9301_regs_t;

typedef enum{
	READ,
	WRITE,

	SET_GAIN,
	SET_INTEGTIME,

	ENABLE,
	DISABLE,

	LOW_GAIN,
	HIGH_GAIN,

	INTEG0,
	INTEG1,
	INTEG2,
	INTEG3,

	NA
}op_t;

#define POWERUP 	0x03
#define POWERDOWN 	0x00

int initialize_light(void);
void command_reg(int file_handler, adps9301_regs_t adps9301_reg, op_t op);
void control_reg(int file_handler, op_t op1, op_t op2, char *buffer);
void timing_reg(int file_handler, op_t op1, op_t op2, op_t op3, char *buffer );
void interrup_reg(int file_handler, op_t op);
void id_reg_read(int file_handler);
void interrupt_thresh_reg(int file_handler, op_t op, char *buffer);
uint16_t adc_data_read(int file_handler, int channel);
float report_lumen(uint16_t adc_data_ch0, uint16_t adc_data_ch1);

typedef enum{
	NIGHT,
	DAY
}tod_t;

tod_t report_tod(int file_handler);

#define lum_NIGHT 10







