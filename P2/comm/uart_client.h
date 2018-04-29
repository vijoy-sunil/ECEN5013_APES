/*
 * i2c_client.h
 *
 *  Created on: Apr 27, 2018
 *      Author: VIJOY-PC
 */

#ifndef COMM_UART_CLIENT_H_
#define COMM_UART_CLIENT_H_

#include "../main.h"
void ConfigureUART_client(void);
void uart_send_data(uint32_t ui32UARTBase, const uint8_t *pui8Buffer, uint32_t ui32Count);



#endif /* COMM_UART_CLIENT_H_ */
