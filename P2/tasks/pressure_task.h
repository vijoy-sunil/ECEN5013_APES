/*
 * pressure_task.h
 *
 *  Created on: Apr 12, 2018
 *      Author: VIJOY-PC
 */

#ifndef TASKS_PRESSURE_TASK_H_
#define TASKS_PRESSURE_TASK_H_

void pressure_task(void *pvParameters);
extern QueueHandle_t pressure_hb_Queue;

#define PRESSURE_SEND_HB                (0x00000022)
#define PRESSURE_COMM_QUEUE_LENGTH      (1)

#define PRESSURE_SLAVE_ADDR                     (0x60)
#define PRESSURE_SLAVE_CONFIG_CNTRL             (0x26)
#define PRESSURE_SLAVE_BAR                      (0x39)

#define MSB_P                (0x01)
#define CSB_P                (0x02)
#define LSB_P                (0x03)

#endif /* TASKS_PRESSURE_TASK_H_ */
