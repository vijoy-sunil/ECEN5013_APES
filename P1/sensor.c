#include "tmp102.h"
#include "adps9301.h"


void main()
{
	// int temp = initialize_temp();
	// char buffer[2];
	// temperature_read(temp,buffer);

	// printf("FLOAT TEMP CEL %f KEL %f FAR %f\n",temp_conv(CELCIUS,buffer),temp_conv(KELVIN,buffer),temp_conv(FARENHEIT,buffer));
	
	// configreg_write(temp,buffer);

	// buffer[0]=0;
	// configreg_read(temp,buffer);
	// printf("CONFIG %x\n",buffer[0]);

	// tlow_read(temp,buffer);
	// printf("TLOW %x\n",buffer[0]);

	// tlow_write(temp,buffer);

	// tlow_read(temp,buffer);
	// printf("TLOW NEW %x\n",buffer[0]);

	// thigh_read(temp,buffer);
	// printf("THIGH %x\n",buffer[0]);

	// thigh_write(temp,buffer);

	// thigh_read(temp,buffer);
	// printf("THIGH NEW %x\n",buffer[0]);

	int light;
	char lightbuffer[1];
	uint16_t ch0,ch1;
	light=initialize_light();

	//TURN OFF LIGHT
	//command_reg(light,CONTROL,WRITE);
	//control_reg(light,WRITE,DISABLE,lightbuffer);
	
	//lightbuffer[0]=0;

	//control_reg(light,READ,ENABLE,lightbuffer);

	//TURN ON SENSOR
	//command_reg(light,CONTROL,WRITE);
	//control_reg(light,WRITE,ENABLE,lightbuffer);
	
	//lightbuffer[0]=0;

	//control_reg(light,READ,ENABLE,lightbuffer);

	//printf("CONTROL %x\n",lightbuffer[0]);

	// command_reg(light,ID,WRITE);
	// id_reg_read(light);

	// //TIMING REG
	// command_reg(light,TIMING,WRITE);
	// timing_reg(light, WRITE,SET_GAIN, HIGH_GAIN, lightbuffer);
	// // command_reg(light,TIMING,WRITE);
	// timing_reg(light, WRITE,SET_INTEGTIME, INTEG0, lightbuffer);

	// //command_reg(light,TIMING,WRITE);
	// timing_reg(light, READ,NA, NA, lightbuffer)	;
	// printf("TIMING %x\n",lightbuffer[0] );

	//READ CHANNEL VALUES
	ch0=adc_data_read(light,0);
	ch1=adc_data_read(light,1);
	//git sprintf("CH-0 %d CH-1 %d\n",ch0,ch1);
	printf("LUMEN %f\n",report_lumen(ch0, ch1));

	printf("%d\n", report_tod(light));


}