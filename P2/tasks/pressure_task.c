#include "../main.h"
#include "./tasks/pressure_task.h"
#include "./comms/i2c.h"
#include "./utils/pack_msg.h"
#include "./tasks/comm_task.h"

QueueHandle_t pressure_comm_Queue = NULL;
extern TaskHandle_t mainTask_handle;

void pressure_task(void *pvParameters)
{
    BaseType_t ret;

    //data transfer queue
    pressure_comm_Queue = xQueueCreate( PRESSURE_COMM_QUEUE_LENGTH, sizeof(unsigned char));

    uint32_t pressure_ReceivedValue, signal;
    while(1)
    {
        //UARTprintf("PRESSURE running\r\n");
        //functions
        //send heartbeat periodically
        xTaskNotifyWait(0xFFFFFFFF,
                        pressure_ReceivedValue,
                        &pressure_ReceivedValue,
                        portMAX_DELAY);

        //get sensor data
        //while(1);

        if(pressure_ReceivedValue & PRESSURE_REQUEST_HB){
            pressure_ReceivedValue = 0;
            signal = PRESSURE_SEND_HB;
            xTaskNotify(mainTask_handle, signal, eSetBits);
            //UARTprintf("PRESSURE task send hb\r\n");
        }


    }
}



