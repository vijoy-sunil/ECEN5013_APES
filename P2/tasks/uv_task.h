/*
 * uv_task.h
 *
 *  Created on: Apr 12, 2018
 *      Author: VIJOY-PC
 */

#ifndef TASKS_UV_TASK_H_
#define TASKS_UV_TASK_H_


void uv_task(void *pvParameters);
extern QueueHandle_t uv_hb_Queue;

#define UV_SEND_HB              (0x11)
#define UV_COMM_QUEUE_LENGTH    (2)

#endif /* TASKS_UV_TASK_H_ */
