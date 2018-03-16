/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/16/2018
* DESCRITPTION	: 
                  
* HEADER FILES	: cmocka_tests.h
****************************************************************/

#include "cmocka_tests.h"

void test_temperature_conv(void **state)
{
	char buffer[2];

	//mock data
	buffer[0] = 0x32;
	buffer[1] = 0x00;

	float expected = 50.0;
	float result = temp_conv(CELCIUS, buffer);
	assert_int_equal((int)expected, (int)result);
}

void test_light_conv(void **state)
{
	uint16_t adc_data_ch0, adc_data_ch1;

	//mock data
	adc_data_ch0 = 0x0011;
	adc_data_ch1 = 0x0011;

	float expected = ;
	float result = report_lumen(adc_data_ch0,adc_data_ch1);

	assert_int_equal((int)expected, (int)result);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_temperature_conv),
		cmocka_unit_test(test_light_conv)
		
	};

	return cmocka_run_group_tests(tests,NULL,NULL);
}

