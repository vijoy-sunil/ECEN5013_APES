/*
 * comm_task.h
 *
 *  Created on: Apr 12, 2018
 *      Author: VIJOY-PC
 */

#ifndef TASKS_COMM_TASK_H_
#define TASKS_COMM_TASK_H_

void comm_task(void *pvParameters);
void ConfigureUART_client(void);
void uart_send_data(uint32_t ui32UARTBase, const uint8_t *pui8Buffer, uint32_t ui32Count);
void ConfigureUART(void);

extern QueueHandle_t comm_hb_Queue;
#define COMM_SOCKET_QUEUE_LENGTH    (1)

extern uint32_t output_clock_rate_hz;



#endif /* TASKS_COMM_TASK_H_ */
