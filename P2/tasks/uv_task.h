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

#define UV_SEND_HB              (0x00000011)
#define UV_COMM_QUEUE_LENGTH    (1)

#define UV_SLAVE_ADDR           (0x38)
#define UV_SLAVE_MSB            (0x38)
#define UV_SLAVE_LSB            (0x39)

#define IT_1                    (0x01) //1T   180 ms
#define UV_CMD_WDM              (0x02)

#endif /* TASKS_UV_TASK_H_ */
