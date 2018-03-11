#include "tmp102.h"
// #include "i2c_wrap.h"


void main()
{
	int temp = initialize_temp();
	char buffer[2];
	temperature_read(temp,buffer);

	printf("FLOAT TEMP CEL %f KEL %f FAR %f\n",temp_conv(CELCIUS,buffer),temp_conv(KELVIN,buffer),temp_conv(FARENHEIT,buffer));
	
	configreg_write(temp,buffer);

	buffer[0]=0;
	configreg_read(temp,buffer);
	printf("CONFIG %x\n",buffer[0]);

	tlow_read(temp,buffer);
	printf("TLOW %x\n",buffer[0]);

	tlow_write(temp,buffer);

	tlow_read(temp,buffer);
	printf("TLOW NEW %x\n",buffer[0]);

	thigh_read(temp,buffer);
	printf("THIGH %x\n",buffer[0]);

	thigh_write(temp,buffer);

	thigh_read(temp,buffer);
	printf("THIGH NEW %x\n",buffer[0]);

}