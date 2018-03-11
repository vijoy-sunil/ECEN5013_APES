#include "tmp102.h"
// #include "i2c_wrap.h"


void main()
{
	printf("Hello");
	
	int temp = initialize_temp();
	char buffer[2];
	temperature_read(temp,buffer);

	printf("FLOAT TEMP CEL %f KEL %f FAR %f\n",temp_conv(CELCIUS,buffer),temp_conv(KELVIN,buffer),temp_conv(FARENHEIT,buffer));

}