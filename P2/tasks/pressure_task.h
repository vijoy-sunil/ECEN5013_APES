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

#define PRESSURE_SEND_HB              (0x22)
#define PRESSURE_COMM_QUEUE_LENGTH    (2)

#endif /* TASKS_PRESSURE_TASK_H_ */
