#include <comms/i2c_drive.h>
#include "../main.h"
#include "./tasks/uv_task.h"
#include "./utils/pack_msg.h"
#include "./tasks/comm_task.h"
#include "./comms/i2c_drive.h"


QueueHandle_t uv_comm_Queue = NULL;
extern TaskHandle_t mainTask_handle;

int initialize_uv(void){
    int ret = 0;
    uint32_t packet[1];
    // clear ACK bit
    packet[0] = ((IT_1<<2) | UV_CMD_WDM);
    sendI2C(packet, 1, UV_SLAVE_ADDR);

    //write 0x06 into 0x70 address
    packet[0] = 0x06;
    sendI2C(packet, 1, UV_SLAVE_ADDR);

    ret = 1;
    return ret;
}

void uv_task(void *pvParameters)
{
    //BaseType_t ret;
    //data transfer queue
    uv_comm_Queue = xQueueCreate( UV_COMM_QUEUE_LENGTH, sizeof(msg_pack_t) );

    uint32_t uv_ReceivedValue, signal;
    uint8_t uv_data[2];

    msg_pack_t* uv_packet = (msg_pack_t*)malloc(sizeof(msg_pack_t));

    //initialize uv sensor here
    //startup test
    int ret = initialize_uv();
    if(ret)
        UARTprintf("UV sensor init SUCCESS\n");
    else
        UARTprintf("UV sensor init ERROR");

    uint32_t packet[1];
    while(1)
    {
        //functions
        //send heartbeat periodically
        xTaskNotifyWait(0xFFFFFFFF,
                        uv_ReceivedValue,
                        &uv_ReceivedValue,
                        portMAX_DELAY);

        //get sensor data 2 * 8 bit
        receiveI2C(packet, 1, UV_SLAVE_MSB);
        uv_data[1] = (uint8_t)(packet[0]);

        receiveI2C(packet, 1, UV_SLAVE_LSB);
        uv_data[0] = (uint8_t)(packet[0]);

        //pack data
        uv_packet->source = SENSOR_UV;
        uv_packet->payload = uv_data[0] | (uv_data[1] << 8);

        //send data to comm task
        xQueueSend( uv_comm_Queue, uv_packet, portMAX_DELAY);

        if(uv_ReceivedValue & UV_REQUEST_HB){
            uv_ReceivedValue = 0;
            signal = UV_SEND_HB;
            xTaskNotify(mainTask_handle, signal, eSetBits);
            //UARTprintf("UV task send hb\r\n");
        }


    }
}
