/*
 * i2c.h
 *
 *  Created on: Apr 12, 2018
 *      Author: VIJOY-PC
 */

#ifndef COMMS_I2C_DRIVE_H_
#define COMMS_I2C_DRIVE_H_

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/i2c.h"

#include <stdint.h>

void ConfigureI2C(void);
void sendI2C(uint8_t* packet, uint8_t num_of_args, uint8_t slave_addr);
uint8_t get_data_from_uv(uint8_t slave_addr);
uint8_t  get_data_from_pressure(uint8_t reg, uint8_t slave_addr);

#endif /* COMMS_I2C_DRIVE_H_ */
