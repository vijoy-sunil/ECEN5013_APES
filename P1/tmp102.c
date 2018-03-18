#include "tmp102.h"

void tlowRead(int i2c_file_handler, char *buffer) {
  char P1P0 = TEMP_TLOW_REG;
  i2cWrite(i2c_file_handler, &P1P0, 1);
  i2cRead(i2c_file_handler, buffer, 2);
}

void thighRead(int i2c_file_handler, char *buffer) {
  char P1P0 = TEMP_THIGH_REG;
  i2cWrite(i2c_file_handler, &P1P0, 1);
  i2cRead(i2c_file_handler, buffer, 2);
}

void tlowWrite(int i2c_file_handler) {
  char buffer[3];

  buffer[0] = TEMP_TLOW_REG;
  buffer[1] = TEMP_TLOW_VAL_B1;
  buffer[2] = TEMP_TLOW_VAL_B2;
  i2cWrite(i2c_file_handler, buffer, 3);
}

void thighWrite(int i2c_file_handler) {
  char buffer[3];

  buffer[0] = TEMP_THIGH_REG;
  buffer[1] = TEMP_THIGH_VAL_B1;
  buffer[2] = TEMP_THIGH_VAL_B2;
  i2cWrite(i2c_file_handler, buffer, 3);
}

void temperatureRead(int i2c_file_handler, char *buffer) {
  char P1P0 = TEMP_READ_REG;
  i2cWrite(i2c_file_handler, &P1P0, 1);
  i2cRead(i2c_file_handler, buffer, 2);
}

int Temp_sensor_init() {
  int temp;
  temp = i2cInit("/dev/i2c-2", temp, TEMP_ADDR);
  return temp;
}

void configRegWrite(int file_handler, char *buffer) {
  char config[3];
  config[0] = TEMP_CONFIG_REG;
  config[1] = buffer[0];
  config[2] = buffer[1];
  i2cWrite(file_handler, config, 3);
}

void configRegRead(int file_handler, char *buffer) {
  char P1P0 = TEMP_CONFIG_REG;
  i2cWrite(file_handler, &P1P0, 1);
  i2cRead(file_handler, buffer, 2);
}

float covert_temperature(temp_unit unit, char *buffer) {
  float temperature;
  unsigned char MSB, LSB;
  int temp_12b;
  // Reference: http://bildr.org/2011/01/tmp102-arduino
  MSB = buffer[0];
  LSB = buffer[1];
  // 12 bit result
  temp_12b = ((MSB << 8) | LSB);
  int negative = temp_12b & 0x8000;
  float multiplier = 0.0625;
  temp_12b = temp_12b >> 4;
  if (negative == 0x8000) {
    temp_12b = 0x0FFF - (temp_12b - 1);
    multiplier = -0.0625;
  }
  switch (unit) {
  case CELCIUS:
    temperature = temp_12b * multiplier;
    break;

  case FARENHEIT:
    temperature = 1.8 * (temp_12b * multiplier) + 32;
    break;

  case KELVIN:
    temperature = 273.15 + (temp_12b * multiplier);
    break;

  default:
    temperature = temp_12b * multiplier;
  }
  return temperature;
}
