#include "../main.h"
#include "./tasks/uv_task.h"
#include "./comms/i2c.h"
#include "./utils/pack_msg.h"
#include "./tasks/comm_task.h"

QueueHandle_t uv_comm_Queue = NULL;
extern TaskHandle_t mainTask_handle;

void uv_task(void *pvParameters)
{
    BaseType_t ret;
    //data transfer queue
    uv_comm_Queue = xQueueCreate( UV_COMM_QUEUE_LENGTH, sizeof(unsigned char) );

    uint32_t uv_ReceivedValue, signal;
    while(1)
    {
        //functions
        //send heartbeat periodically
        xTaskNotifyWait(0xFFFFFFFF,
                        uv_ReceivedValue,
                        &uv_ReceivedValue,
                        portMAX_DELAY);

        //get sensor data
        //while(1);

        if(uv_ReceivedValue & UV_REQUEST_HB){
            uv_ReceivedValue = 0;
            signal = UV_SEND_HB;
            xTaskNotify(mainTask_handle, signal, eSetBits);
            //UARTprintf("UV task send hb\r\n");
        }


    }
}
